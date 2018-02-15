/*
 * ADSpinnaker.cpp
 *
 * This is a driver for FLIR (PointGrey) GigE, 10GigE, and USB3 cameras using their Spinnaker SDK
 *
 * Author: Mark Rivers
 *         University of Chicago
 *
 * Created: February 3, 2018
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <set>

#include <epicsEvent.h>
#include <epicsTime.h>
#include <epicsThread.h>
#include <epicsMessageQueue.h>
#include <iocsh.h>
#include <cantProceed.h>
#include <epicsString.h>
#include <epicsExit.h>

#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
using namespace std;

#include "ADDriver.h"

#include <epicsExport.h>

#define DRIVER_VERSION      1
#define DRIVER_REVISION     0
#define DRIVER_MODIFICATION 0

static const char *driverName = "ADSpinnaker";

/* Spinnaker driver specific parameters */
#define SPTriggerSourceString         "SP_TRIGGER_SOURCE"
#define SPTriggerActivationString     "SP_TRIGGER_ACTIVATION"
#define SPSoftwareTriggerString       "SP_SOFTWARE_TRIGGER"
#define SPTriggerDelayString          "SP_TRIGGER_DELAY"
#define SPPixelFormatString           "SP_PIXEL_FORMAT"
#define SPVideoModeString             "SP_VIDEO_MODE"
#define SPConvertPixelFormatString    "SP_CONVERT_PIXEL_FORMAT"
#define SPAcquisitionModeString       "SP_ACQUISITION_MODE"
#define SPFrameRateString             "SP_FRAME_RATE"
#define SPFrameRateEnableString       "SP_FRAME_RATE_ENABLE"
#define SPFrameRateAutoString         "SP_FRAME_RATE_AUTO"
#define SPAcquisitionStatusString     "SP_ACQUISITION_STATUS"
#define SPAcquisitionModeString       "SP_ACQUISITION_MODE"

/*
#define PGSkipFramesString            "PG_SKIP_FRAMES"
#define PGStrobeSourceString          "PG_STROBE_SOURCE"
#define PGStrobePolarityString        "PG_STROBE_POLARITY"
#define PGStrobeEnableString          "PG_STROBE_ENABLE"
#define PGStrobeDelayString           "PG_STROBE_DELAY"
#define PGStrobeDurationString        "PG_STROBE_DURATION"
#define PGPacketSizeString            "PG_PACKET_SIZE"
#define PGPacketSizeActualString      "PG_PACKET_SIZE_ACTUAL"
#define PGMaxPacketSizeString         "PG_MAX_PACKET_SIZE"
#define PGPacketDelayString           "PG_PACKET_DELAY"
#define PGPacketDelayActualString     "PG_PACKET_DELAY_ACTUAL"
#define PGBandwidthString             "PG_BANDWIDTH"
#define PGTimeStampModeString         "PG_TIME_STAMP_MODE"
#define PGCorruptFramesString         "PG_CORRUPT_FRAMES"
#define PGDriverDroppedString         "PG_DRIVER_DROPPED"
#define PGTransmitFailedString        "PG_TRANSMIT_FAILED"
#define PGDroppedFramesString         "PG_DROPPED_FRAMES"
*/

// Default packet delay in microseconds
#define DEFAULT_PACKET_DELAY 400

// Size of message queue for callback function
#define CALLBACK_MESSAGE_QUEUE_SIZE 10

typedef enum {
    SPPropertyTypeInt,
    SPPropertyTypeBoolean,
    SPPropertyTypeEnum,
    SPPropertyTypeDouble,
    SPPropertyTypeString,
    SPPropertyTypeCmd
} SPPropertyType_t;

#define MAX_ENUM_STRING_SIZE 26
typedef struct {
    int value;
    char string[MAX_ENUM_STRING_SIZE];
} enumStruct_t;


/*
static const char *propertyTypeStrings[] = {
    "Brightness",
    "AutoExposuure",
    "Sharpness",
    "WhiteBalance",
    "Hue",
    "Saturation",
    "Gamma",

};

static const char *gigEPropertyTypeStrings[NUM_GIGE_PROPERTIES] = {
    "Heartbeat",
    "HeartbeatTimeout",
    "PacketSize",
    "PacketDelay"
};

*/

typedef enum {
   TimeStampCamera,
   TimeStampEPICS,
   TimeStampHybrid
} PGTimeStamp_t;

class propertyListElement {
    public:
        propertyListElement(SPPropertyType_t propertyType, const char *nodeName)
          : propertyType_(propertyType), nodeName_(nodeName) {}
        SPPropertyType_t propertyType_;
        const char *nodeName_;
};

class ImageEventHandler : public ImageEvent
{
public:

	ImageEventHandler(epicsMessageQueue *pMsgQ) 
	 : pMsgQ_(pMsgQ)
	{
	}
	~ImageEventHandler() {}

	void OnImageEvent(ImagePtr image)
	{
      static ImagePtr imageCopy;
      imageCopy = image;

      if (pMsgQ_->send(&imageCopy, sizeof(imageCopy)) != 0) {
          printf("OnImageEvent error calling pMsgQ_->send()\n");
      }
	}
	
private:
  epicsMessageQueue *pMsgQ_;

};

/** Main driver class inherited from areaDetectors ADDriver class.
 * One instance of this class will control one camera.
 */
class ADSpinnaker : public ADDriver
{
public:
    ADSpinnaker(const char *portName, int cameraId, int traceMask, int memoryChannel,
                 int maxBuffers, size_t maxMemory,
                 int priority, int stackSize);

    // virtual methods to override from ADDriver
    virtual asynStatus writeInt32( asynUser *pasynUser, epicsInt32 value);
    virtual asynStatus writeFloat64( asynUser *pasynUser, epicsFloat64 value);
    virtual asynStatus readEnum(asynUser *pasynUser, char *strings[], int values[], int severities[], 
                                size_t nElements, size_t *nIn);
//    void report(FILE *fp, int details);
    /**< These should be private but are called from C callback functions, must be public. */
    void imageGrabTask();
    void shutdown();

protected:
    int SPTriggerSource;          /** Trigger source                                  (int32 write/read) */
    #define FIRST_SP_PARAM SPTriggerSource
    int SPTriggerActivation;      /** Trigger activation (polarity)                   (int32 write/read) */
    int SPSoftwareTrigger;        /** Issue a software trigger                        (int32 write/read) */
    int SPTriggerDelay;           /** Trigger delay                                   (float64 write/read) */
    int SPPixelFormat;            /** Pixel format (Mono8, Mono16, etc.)              (int32 read/write) */
    int SPVideoMode;              /** Video mode                                      (int32 read/write) */
    int SPConvertPixelFormat;     /** The pixel format to convert to                  (int32 read/write) */
    int SPFrameRate;              /** Frame rate                                      (float64 read/write) */
    int SPFrameRateEnable;        /** Frame rate enable/disable                       (int32 read/write) */
    int SPFrameRateAuto;          /** Frame rate auto enable/disable                  (int32 read/write) */
    int SPAcquisitionStatus;      /** Acquisition status                              (int32 read/write) */
    int SPAcquisitionMode;        /** Acquisition mode                                (int32 read/write) */

//    int PGSkipFrames;             /** Frames to skip in trigger mode 3                (int32 write/read) */
//    int PGStrobeSource;           /** Strobe source GPIO pin                          (int32 write/read) */
//    int PGStrobePolarity;         /** Strobe polarity (low/high)                      (int32 write/read) */
//    int PGStrobeEnable;           /** Strobe enable/disable strobe                    (int32 write/read) */
//    int PGStrobeDelay;            /** Strobe delay                                    (float64 write/read) */
//    int PGStrobeDuration;         /** Strobe duration                                 (float64 write/read) */
//    int PGPacketSize;             /** Size of data packets from camera                (int32 write/read) */
//    int PGPacketSizeActual;       /** Size of data packets from camera                (int32 write/read) */
//    int PGMaxPacketSize;          /** Maximum size of data packets from camera        (int32 write/read) */
//    int PGPacketDelay;            /** Packet delay in usec from camera, GigE only     (int32 write/read) */
//    int PGPacketDelayActual;      /** Packet delay in usec from camera, GigE only     (int32 read) */
//    int PGBandwidth;              /** Bandwidth in MB/s                               (float64 read) */
//    int PGTimeStampMode;          /** Time stamp mode (PGTimeStamp_t)                 (int32 write/read) */
//    int PGCorruptFrames;          /** Number of corrupt frames                        (int32 read) */
//    int PGDriverDropped;          /** Number of driver dropped frames                 (int32 read) */
//    int PGTransmitFailed;         /** Number of transmit failures                     (int32 read) */
//    int PGDroppedFrames;          /** Number of dropped frames                        (int32 read) */

private:
    /* Local methods to this class */
    asynStatus grabImage();
    asynStatus startCapture();
    asynStatus stopCapture();

    asynStatus connectCamera();
    asynStatus disconnectCamera();
    asynStatus readStatus();

    propertyListElement* findProperty(int paramIndex);

    /* camera property control functions */
    asynStatus setSPProperty (int paramIndex, void *value=0, void *readbackValue=0, bool setParam=true);
    asynStatus getSPProperty (int paramIndex, void *value=0, bool setParam=true);

    asynStatus setImageParams();
    
    void imageEventCallback(ImagePtr pImage);

/*
    asynStatus getAllProperties();
    asynStatus setTrigger();
    asynStatus setStrobe();
*/
    /* Data */
    int cameraId_;
    int memoryChannel_;
    
    INodeMap *pNodeMap_;    
    SystemPtr system_;
    CameraList camList_;
    CameraPtr pCamera_;
    ImagePtr pImage_;
    ImageEventHandler *pImageEventHandler_;
    std::map<int, propertyListElement*> propertyList_;

    int exiting_;
    epicsEventId startEventId_;
    epicsMessageQueue *pCallbackMsgQ_;
    NDArray *pRaw_;
};


/** Number of asynPortDriver parameters this driver supports. */
#define NUM_PG_PARAMS ((int)(&LAST_PG_PARAM - &FIRST_PG_PARAM + 1))

/** Configuration function to configure one camera.
 *
 * This function need to be called once for each camera to be used by the IOC. A call to this
 * function instanciates one object from the ADSpinnaker class.
 * \param[in] portName asyn port name to assign to the camera.
 * \param[in] cameraId The camera index or serial number; <1000 is assumed to be index, >=1000 is assumed to be serial number.
 * \param[in] traceMask The initial value of the asynTraceMask.  
 *            If set to 0 or 1 then asynTraceMask will be set to ASYN_TRACE_ERROR.
 *            If set to 0x21 (ASYN_TRACE_WARNING | ASYN_TRACE_ERROR) then each call to the
 *            FlyCap2 library will be traced including during initialization.
 * \param[in] memoryChannel  The camera memory channel (non-volatile memory containing camera parameters) 
 *            to load during initialization.  If 0 no memory channel is loaded.
 *            If >=1 thenRestoreFromMemoryChannel(memoryChannel-1) is called.  
 *            Set memoryChannel to 1 to work around a bug in the Linux GigE driver in R2.0.
 * \param[in] maxBuffers Maxiumum number of NDArray objects (image buffers) this driver is allowed to allocate.
 *            This driver requires 2 buffers, and each queue element in a plugin can require one buffer
 *            which will all need to be added up in this parameter. 0=unlimited.
 * \param[in] maxMemory Maximum memory (in bytes) that this driver is allowed to allocate. So if max. size = 1024x768 (8bpp)
 *            and maxBuffers is, say 14. maxMemory = 1024x768x14 = 11010048 bytes (~11MB). 0=unlimited.
 * \param[in] priority The EPICS thread priority for this driver.  0=use asyn default.
 * \param[in] stackSize The size of the stack for the EPICS port thread. 0=use asyn default.
 */
extern "C" int ADSpinnakerConfig(const char *portName, int cameraId, int traceMask, int memoryChannel, 
                               int maxBuffers, size_t maxMemory, int priority, int stackSize)
{
    new ADSpinnaker( portName, cameraId, traceMask, memoryChannel, maxBuffers, maxMemory, priority, stackSize);
    return asynSuccess;
}


static void c_shutdown(void *arg)
{
  ADSpinnaker *p = (ADSpinnaker *)arg;
  p->shutdown();
}


static void imageGrabTaskC(void *drvPvt)
{
    ADSpinnaker *pPvt = (ADSpinnaker *)drvPvt;

    pPvt->imageGrabTask();
}

/** Constructor for the ADSpinnaker class
 * \param[in] portName asyn port name to assign to the camera.
 * \param[in] cameraId The camera index or serial number; <1000 is assumed to be index, >=1000 is assumed to be serial number.
 * \param[in] traceMask The initial value of the asynTraceMask.  
 *            If set to 0 or 1 then asynTraceMask will be set to ASYN_TRACE_ERROR.
 *            If set to 0x21 (ASYN_TRACE_WARNING | ASYN_TRACE_ERROR) then each call to the
 *            FlyCap2 library will be traced including during initialization.
 * \param[in] memoryChannel  The camera memory channel (non-volatile memory containing camera parameters) 
 *            to load during initialization.  If 0 no memory channel is loaded.
 *            If >=1 thenRestoreFromMemoryChannel(memoryChannel-1) is called.  
 *            Set memoryChannel to 1 to work around a bug in the Linux GigE driver in R2.0.
 * \param[in] maxBuffers Maxiumum number of NDArray objects (image buffers) this driver is allowed to allocate.
 *            This driver requires 2 buffers, and each queue element in a plugin can require one buffer
 *            which will all need to be added up in this parameter. 0=unlimited.
 * \param[in] maxMemory Maximum memory (in bytes) that this driver is allowed to allocate. So if max. size = 1024x768 (8bpp)
 *            and maxBuffers is, say 14. maxMemory = 1024x768x14 = 11010048 bytes (~11MB). 0=unlimited.
 * \param[in] priority The EPICS thread priority for this driver.  0=use asyn default.
 * \param[in] stackSize The size of the stack for the EPICS port thread. 0=use asyn default.
 */
ADSpinnaker::ADSpinnaker(const char *portName, int cameraId, int traceMask, int memoryChannel,
                    int maxBuffers, size_t maxMemory, int priority, int stackSize )
    : ADDriver(portName, 1, 0, maxBuffers, maxMemory,
            asynEnumMask, asynEnumMask,
            ASYN_CANBLOCK | ASYN_MULTIDEVICE, 1, priority, stackSize),
    cameraId_(cameraId), memoryChannel_(memoryChannel), exiting_(0), pRaw_(NULL)
{
    static const char *functionName = "ADSpinnaker";
//    int i;
    //PropertyType propType;
    asynStatus status;
    
    if (traceMask == 0) traceMask = ASYN_TRACE_ERROR;
    pasynTrace->setTraceMask(pasynUserSelf, traceMask);

    createParam(SPTriggerSourceString,          asynParamInt32,   &SPTriggerSource);
    createParam(SPTriggerActivationString,      asynParamInt32,   &SPTriggerActivation);
    createParam(SPSoftwareTriggerString,        asynParamInt32,   &SPSoftwareTrigger);
    createParam(SPTriggerDelayString,           asynParamFloat64, &SPTriggerDelay);
    createParam(SPPixelFormatString,            asynParamInt32,   &SPPixelFormat);
    createParam(SPVideoModeString,              asynParamInt32,   &SPVideoMode);
    createParam(SPConvertPixelFormatString,     asynParamInt32,   &SPConvertPixelFormat);
    createParam(SPFrameRateString,              asynParamFloat64, &SPFrameRate);
    createParam(SPFrameRateEnableString,        asynParamInt32,   &SPFrameRateEnable);
    createParam(SPFrameRateAutoString,          asynParamInt32,   &SPFrameRateAuto);
    createParam(SPAcquisitionStatusString,      asynParamInt32,   &SPAcquisitionStatus);
    createParam(SPAcquisitionModeString,        asynParamInt32,   &SPAcquisitionMode);

/*
    createParam(PGSkipFramesString,             asynParamInt32,   &PGSkipFrames);
    createParam(PGStrobeSourceString,           asynParamInt32,   &PGStrobeSource);
    createParam(PGStrobePolarityString,         asynParamInt32,   &PGStrobePolarity);
    createParam(PGStrobeEnableString,           asynParamInt32,   &PGStrobeEnable);
    createParam(PGStrobeDelayString,            asynParamFloat64, &PGStrobeDelay);
    createParam(PGStrobeDurationString,         asynParamFloat64, &PGStrobeDuration);
    createParam(PGPacketSizeString,             asynParamInt32,   &PGPacketSize);
    createParam(PGPacketSizeActualString,       asynParamInt32,   &PGPacketSizeActual);
    createParam(PGMaxPacketSizeString,          asynParamInt32,   &PGMaxPacketSize);
    createParam(PGPacketDelayString,            asynParamInt32,   &PGPacketDelay);
    createParam(PGPacketDelayActualString,      asynParamInt32,   &PGPacketDelayActual);
    createParam(PGBandwidthString,              asynParamFloat64, &PGBandwidth);
    createParam(PGTimeStampModeString,          asynParamInt32,   &PGTimeStampMode);
    createParam(PGCorruptFramesString,          asynParamInt32,   &PGCorruptFrames);
    createParam(PGDriverDroppedString,          asynParamInt32,   &PGDriverDropped);
    createParam(PGTransmitFailedString,         asynParamInt32,   &PGTransmitFailed);
    createParam(PGDroppedFramesString,          asynParamInt32,   &PGDroppedFrames);
*/
    /* Set initial values of some parameters */
    setIntegerParam(NDDataType, NDUInt8);
    setIntegerParam(NDColorMode, NDColorModeMono);
    setIntegerParam(NDArraySizeZ, 0);
    setIntegerParam(ADMinX, 0);
    setIntegerParam(ADMinY, 0);
    setStringParam(ADStringToServer, "<not used by driver>");
    setStringParam(ADStringFromServer, "<not used by driver>");
    setIntegerParam(SPTriggerSource, 0);
//    setIntegerParam(PGStrobeSource, 1);
//    setIntegerParam(PGBinningMode, 1);

    // Retrieve singleton reference to system object
    system_ = System::GetInstance();

    status = connectCamera();
    if (status) {
        asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
            "%s:%s:  camera connection failed (%d)\n",
            driverName, functionName, status);
        // Call report() to get a list of available cameras
        report(stdout, 1);
        return;
    }

    // Construct property list.  This is a convenience.
    propertyList_[ADSerialNumber]       = new propertyListElement(SPPropertyTypeString, "DeviceSerialNumber");
    propertyList_[ADFirmwareVersion]    = new propertyListElement(SPPropertyTypeString, "DeviceFirmwareVersion");
    propertyList_[ADManufacturer]       = new propertyListElement(SPPropertyTypeString, "DeviceVendorName");
    propertyList_[ADModel]              = new propertyListElement(SPPropertyTypeString, "DeviceModelName");

    propertyList_[ADMaxSizeX]           = new propertyListElement(SPPropertyTypeInt,    "WidthMax");
    propertyList_[ADMaxSizeY]           = new propertyListElement(SPPropertyTypeInt,    "HeightMax");
    propertyList_[ADSizeX]              = new propertyListElement(SPPropertyTypeInt,    "Width");
    propertyList_[ADSizeY]              = new propertyListElement(SPPropertyTypeInt,    "Height");
    propertyList_[ADMinX]               = new propertyListElement(SPPropertyTypeInt,    "OffsetX");
    propertyList_[ADMinY]               = new propertyListElement(SPPropertyTypeInt,    "OffsetY");
    propertyList_[ADBinX]               = new propertyListElement(SPPropertyTypeInt,    "BinningHorizontal");
    propertyList_[ADBinY]               = new propertyListElement(SPPropertyTypeInt,    "BinningVertical");
    propertyList_[ADNumImages]          = new propertyListElement(SPPropertyTypeInt,    "AcquisitionFrameCount");

    propertyList_[SPAcquisitionStatus]  = new propertyListElement(SPPropertyTypeBoolean,"AcquisitionStatus");

    propertyList_[SPSoftwareTrigger]    = new propertyListElement(SPPropertyTypeCmd,    "TriggerSoftware");

    propertyList_[ADAcquireTime]        = new propertyListElement(SPPropertyTypeDouble, "ExposureTime");
    propertyList_[ADGain]               = new propertyListElement(SPPropertyTypeDouble, "Gain");
    propertyList_[SPFrameRate]          = new propertyListElement(SPPropertyTypeDouble, "AcquisitionFrameRate");
    propertyList_[SPTriggerDelay]       = new propertyListElement(SPPropertyTypeDouble, "TriggerDelay");
    propertyList_[ADTemperatureActual]  = new propertyListElement(SPPropertyTypeDouble, "DeviceTemperature");

    propertyList_[SPPixelFormat]        = new propertyListElement(SPPropertyTypeEnum,   "PixelFormat");
    propertyList_[SPAcquisitionMode]    = new propertyListElement(SPPropertyTypeEnum,   "AcquisitionMode");
    propertyList_[SPVideoMode]          = new propertyListElement(SPPropertyTypeEnum,   "VideoMode");
    propertyList_[ADTriggerMode]        = new propertyListElement(SPPropertyTypeEnum,   "TriggerMode");
    propertyList_[SPTriggerSource]      = new propertyListElement(SPPropertyTypeEnum,   "TriggerSource");
    propertyList_[SPTriggerActivation]  = new propertyListElement(SPPropertyTypeEnum,   "TriggerActivation");
    propertyList_[SPFrameRateAuto]      = new propertyListElement(SPPropertyTypeEnum,   "AcquisitionFrameRateAuto");
    CNodePtr pBase = (CNodePtr)pNodeMap_->GetNode("AcquisitionFrameRateEnable");
    if (IsAvailable(pBase)) {
        propertyList_[SPFrameRateEnable]   = new propertyListElement(SPPropertyTypeBoolean, "AcquisitionFrameRateEnable");
    } else {
        pBase = (CNodePtr)pNodeMap_->GetNode("AcquisitionFrameRateEnabled");
        if (IsAvailable(pBase)) {
          propertyList_[SPFrameRateEnable] = new propertyListElement(SPPropertyTypeBoolean, "AcquisitionFrameRateEnabled");
        }
    }

		epicsInt32 iValue;
		
		getSPProperty(ADSerialNumber);
 		getSPProperty(ADFirmwareVersion);
 		getSPProperty(ADManufacturer);
 		getSPProperty(ADModel);
 		getSPProperty(ADMaxSizeX, &iValue);
		setIntegerParam(ADSizeX, iValue);
 		getSPProperty(ADMaxSizeY, &iValue);
		setIntegerParam(ADSizeY, iValue);

    // Create the message queue to pass images from the callback class
    pCallbackMsgQ_ = new epicsMessageQueue(CALLBACK_MESSAGE_QUEUE_SIZE, sizeof(ImagePtr));
    if (!pCallbackMsgQ_) {
        cantProceed("ADSpinnaker::ADSpinnaker epicsMessageQueueCreate failure\n");
    }

		pImageEventHandler_ = new ImageEventHandler(pCallbackMsgQ_);
		pCamera_->RegisterEvent(*pImageEventHandler_);

    startEventId_ = epicsEventCreate(epicsEventEmpty);

    // launch image read task
    epicsThreadCreate("PointGreyImageTask", 
                      epicsThreadPriorityMedium,
                      epicsThreadGetStackSize(epicsThreadStackMedium),
                      imageGrabTaskC, this);

    // shutdown on exit
    epicsAtExit(c_shutdown, this);

    return;
}

void ADSpinnaker::shutdown(void)
{
    static const char *functionName = "shutdown";
    exiting_ = 1;
    
    lock();
    printf("Shutting down, calling camList.Clear() and system.ReleaseInstance()\n");
    try {
    		pCamera_->UnregisterEvent(*pImageEventHandler_);
    		delete pImageEventHandler_;
    		pCamera_->DeInit();
        camList_.Clear();
        system_->ReleaseInstance();
    }
    catch (Spinnaker::Exception &e)
    {
    	asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, 
    	    "%s::%s exception %s\n",
    	    driverName, functionName, e.what());
    }
    unlock();
}


asynStatus ADSpinnaker::connectCamera(void)
{
    unsigned int numCameras;
    char tempString[100];
    static const char *functionName = "connectCamera";

    try {
        // Retrieve list of cameras from the system
        camList_ = system_->GetCameras();
    
        numCameras = camList_.GetSize();
    
        asynPrint(pasynUserSelf, ASYN_TRACE_WARNING,
            "%s::%s called camList_.GetSize, camList_=%p, numCameras=%d\n",
            driverName, functionName, &camList_, numCameras);
        
        if (numCameras <= 0) {
            asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, 
                "%s:%s: no cameras found\n",
                driverName, functionName);
     
            // Clear camera list before releasing system
            camList_.Clear();
        
            return asynError;
        }
    
        if (cameraId_ < 1000) {
            asynPrint(pasynUserSelf, ASYN_TRACE_WARNING,
                "%s::%s calling camList_.GetByIndex, camList_=%p\n",
                driverName, functionName, &camList_);
            pCamera_ = camList_.GetByIndex(cameraId_);
        } else { 
            asynPrint(pasynUserSelf, ASYN_TRACE_WARNING,
                "%s::%s calling camList_.GetBySerial, camList_=%p, cameraId_=%d\n",
                driverName, functionName, &camList_, cameraId_);
            char tempString[100];
            sprintf(tempString, "%d", cameraId_);
            std::string tempStdString(tempString);
            pCamera_ = camList_.GetBySerial(tempStdString);
        }
    
//    		report(stdout, 1);
    
    		// Initialize camera
    		pCamera_->Init();
    		
    		// Retrieve GenICam nodemap
    		pNodeMap_ = &pCamera_->GetNodeMap();
    }

    catch (Spinnaker::Exception &e)
    {
    	asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, 
    	    "%s::%s exception %s\n",
    	    driverName, functionName, e.what());
    	return asynError;
    }

    epicsSnprintf(tempString, sizeof(tempString), "%d.%d.%d", 
                  DRIVER_VERSION, DRIVER_REVISION, DRIVER_MODIFICATION);
    setStringParam(NDDriverVersion,tempString);
 
/*   
    Utilities::GetLibraryVersion(&version);
    epicsSnprintf(tempString, sizeof(tempString), "%d.%d.%d", version.major, version.minor, version.type);
    asynPrint(pasynUserSelf, ASYN_TRACE_WARNING,
        "%s::%s called Utilities::GetLibraryVersion, version=%s\n",
        driverName, functionName, tempString);
    setStringParam(ADSDKVersion, tempString);


    // Get and set the embedded image info
    asynPrint(pasynUserSelf, ASYN_TRACE_WARNING,
        "%s::%s calling CameraBase::GetEmbeddedImageInfo, &embeddedInfo=%p\n",
        driverName, functionName, &embeddedInfo);
    error = pCameraBase_->GetEmbeddedImageInfo(&embeddedInfo);
    if (checkError(error, functionName, "GetEmbeddedImageInfo")) return asynError;
    // Force the timestamp and frame counter information to be on
    embeddedInfo.timestamp.onOff = true;
    embeddedInfo.frameCounter.onOff = true;
    asynPrint(pasynUserSelf, ASYN_TRACE_WARNING,
        "%s::%s calling CameraBase::SetEmbeddedImageInfo, &embeddedInfo=%p\n",
        driverName, functionName, &embeddedInfo);
    error = pCameraBase_->SetEmbeddedImageInfo(&embeddedInfo);
    if (checkError(error, functionName, "SetEmbeddedImageInfo")) return asynError;
*/    
    return asynSuccess;
}

/*
asynStatus ADSpinnaker::disconnectCamera(void) 
{
    Error error;
    static const char *functionName = "disconnectCamera";

    if (pCameraBase_->IsConnected()) {
        asynPrint(pasynUserSelf, ASYN_TRACE_WARNING,
            "%s::%s calling CameraBase::Disconnect, pCameraBase_=%p\n",
            driverName, functionName, pCameraBase_);
        error = pCameraBase_->Disconnect();
        if (checkError(error, functionName, "Disconnect")) return asynError;
    }
    printf("%s:%s disconnect camera OK\n", driverName, functionName);
    return asynSuccess;
}
*/

/** Task to grab images off the camera and send them up to areaDetector
 *
 */

void ADSpinnaker::imageGrabTask()
{
    asynStatus status = asynSuccess;
    int imageCounter;
    int numImages, numImagesCounter;
    int imageMode;
    int arrayCallbacks;
    epicsTimeStamp startTime;
    int acquire;
    static const char *functionName = "imageGrabTask";

    lock();

    while (1) {
        // Is acquisition active? 
        getIntegerParam(ADAcquire, &acquire);
        // If we are not acquiring then wait for a semaphore that is given when acquisition is started 
        if (!acquire) {
            setIntegerParam(ADStatus, ADStatusIdle);
            callParamCallbacks();

            // Wait for a signal that tells this thread that the transmission
            // has started and we can start asking for image buffers...
            asynPrint(pasynUserSelf, ASYN_TRACE_FLOW,
                "%s::%s waiting for acquire to start\n", 
                driverName, functionName);
            // Release the lock while we wait for an event that says acquire has started, then lock again
            unlock();
            epicsEventWait(startEventId_);
            lock();
            asynPrint(pasynUserSelf, ASYN_TRACE_FLOW,
                "%s::%s started!\n", 
                driverName, functionName);
            setIntegerParam(ADNumImagesCounter, 0);
            setIntegerParam(ADAcquire, 1);
        }

        // Get the current time 
        epicsTimeGetCurrent(&startTime);
        // We are now waiting for an image
        setIntegerParam(ADStatus, ADStatusWaiting);
        // Call the callbacks to update any changes
        callParamCallbacks();

        status = grabImage();
        if (status == asynError) {
            // remember to release the NDArray back to the pool now
            // that we are not using it (we didn't get an image...)
            if (pRaw_) pRaw_->release();
            pRaw_ = NULL;
            continue;
        }

        getIntegerParam(NDArrayCounter, &imageCounter);
        getIntegerParam(ADNumImages, &numImages);
        getIntegerParam(ADNumImagesCounter, &numImagesCounter);
        getIntegerParam(ADImageMode, &imageMode);
        getIntegerParam(NDArrayCallbacks, &arrayCallbacks);
        imageCounter++;
        numImagesCounter++;
        setIntegerParam(NDArrayCounter, imageCounter);
        setIntegerParam(ADNumImagesCounter, numImagesCounter);

        if (arrayCallbacks) {
            // Call the NDArray callback
            doCallbacksGenericPointer(pRaw_, NDArrayData, 0);
        }
        // Release the NDArray buffer now that we are done with it.
        // After the callback just above we don't need it anymore
        pRaw_->release();
        pRaw_ = NULL;

        // See if acquisition is done if we are in single or multiple mode
        if ((imageMode == ADImageSingle) || ((imageMode == ADImageMultiple) && (numImagesCounter >= numImages))) {
            setIntegerParam(ADStatus, ADStatusIdle);
            status = stopCapture();
        }
        callParamCallbacks();
    }
}

asynStatus ADSpinnaker::grabImage()
{
    asynStatus status = asynSuccess;
    size_t nRows, nCols;
//    size_t stride;
    NDDataType_t dataType;
    NDColorMode_t colorMode;
    int convertPixelFormat;
    int numColors;
    size_t dims[3];
    ImageStatus imageStatus;
    PixelFormatEnums pixelFormat;
    int pixelSize;
    size_t dataSize, dataSizePG;
//    double bandwidth;
//    double frameRate;
    void *pData;
    int nDims;
//    long long timeStamp;
//    int timeStampMode;
    static const char *functionName = "grabImage";

  try {
    unlock();
    if (pCallbackMsgQ_->receive(&pImage_, sizeof(pImage_)) != sizeof(pImage_)) {
        asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
                "%s::%s error receiving from message queue\n",
                driverName, functionName);
        return asynError;
    }
    lock();
    imageStatus = pImage_->GetImageStatus();
    if (imageStatus != IMAGE_NO_ERROR) {
        asynPrint(pasynUserSelf, ASYN_TRACE_WARNING,
            "%s::%s error GetImageStatus returned %d\n",
            driverName, functionName, imageStatus);
        pImage_->Release();
        return asynError;
    } 
    nCols = pImage_->GetWidth();
    nRows = pImage_->GetHeight();
    //stride = pImage_->GetStride();
    pixelFormat = pImage_->GetPixelFormat();
    gcstring pixelFormatName = pImage_->GetPixelFormatName();
 
//    timeStamp = pImage_->GetTimeStamp();    
//    pPGImage = pPGRawImage_;
    // Calculate bandwidth
//    dataSizePG = pPGRawImage_->GetReceivedDataSize();
//    getDoubleParam(FRAME_RATE, PGPropertyValueAbs, &frameRate);
//    bandwidth = frameRate * dataSizePG / (1024 * 1024);
//    setDoubleParam(PGBandwidth, bandwidth);

    // If the incoming pixel format is raw[8,12,16] or mono12 and convertPixelFormat is non-zero then convert
    // the pixel format of the image
    getIntegerParam(SPConvertPixelFormat, &convertPixelFormat);
    if (((pixelFormat == PixelFormat_Raw8)   ||
         (pixelFormat == PixelFormat_Mono12Packed)  ||
         (pixelFormat == PixelFormat_Mono12p) ||
         (pixelFormat == PixelFormat_Mono12) ||
         (pixelFormat == PixelFormat_Raw16)) &&
          convertPixelFormat != 0) {
//        ImagePtr pConvertedImage = pImage_->Convert((PixelFormatEnums)convertPixelFormat);
//        pImage_ = pConvertedImage;
    }
    
     pixelFormat = pImage_->GetPixelFormat();
     pixelFormatName = pImage_->GetPixelFormatName();
     switch (pixelFormat) {
        case PixelFormat_Mono8:
        case PixelFormat_Raw8:
            dataType = NDUInt8;
            colorMode = NDColorModeMono;
            numColors = 1;
            pixelSize = 1;
            break;

        case PixelFormat_RGB8:
            dataType = NDUInt8;
            colorMode = NDColorModeRGB1;
            numColors = 3;
            pixelSize = 1;
            break;

        case PixelFormat_Mono16:
        case PixelFormat_Raw16:
            dataType = NDUInt16;
            colorMode = NDColorModeMono;
            numColors = 1;
            pixelSize = 2;
            break;

        case PixelFormat_RGB16:
            dataType = NDUInt16;
            colorMode = NDColorModeRGB1;
            numColors = 3;
            pixelSize = 2;
            break;

        default:
            asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
                "%s:%s: unsupported pixel format=0x%x\n",
                driverName, functionName, pixelFormat);
            return asynError;
    }

    if (numColors == 1) {
        nDims = 2;
        dims[0] = nCols;
        dims[1] = nRows;
    } else {
        nDims = 3;
        dims[0] = 3;
        dims[1] = nCols;
        dims[2] = nRows;
    }
    dataSize = dims[0] * dims[1] * pixelSize;
    if (nDims == 3) dataSize *= dims[2];
    dataSizePG = pImage_->GetBufferSize();
    // Note, we should be testing for equality here.  However, there appears to be a bug in the
    // SDK when images are converted.  When converting from raw8 to mono8, for example, the
    // size returned by GetDataSize is the size of an RGB8 image, not a mono8 image.
    if (dataSize > dataSizePG) {
        asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
            "%s:%s: data size mismatch: calculated=%lu, reported=%lu\n",
            driverName, functionName, (long)dataSize, (long)dataSizePG);
        //return asynError;
    }
    setIntegerParam(NDArraySizeX, (int)nCols);
    setIntegerParam(NDArraySizeY, (int)nRows);
    setIntegerParam(NDArraySize, (int)dataSize);
    setIntegerParam(NDDataType,dataType);
    if (nDims == 3) {
        colorMode = NDColorModeRGB1;
    } else {
        // If the color mode is currently set to Bayer leave it alone
        getIntegerParam(NDColorMode, (int *)&colorMode);
        if (colorMode != NDColorModeBayer) colorMode = NDColorModeMono;
    }
    setIntegerParam(NDColorMode, colorMode);

    pRaw_ = pNDArrayPool->alloc(nDims, dims, dataType, 0, NULL);
    if (!pRaw_) {
        // If we didn't get a valid buffer from the NDArrayPool we must abort
        // the acquisition as we have nowhere to dump the data...
        setIntegerParam(ADStatus, ADStatusAborting);
        callParamCallbacks();
        asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, 
            "%s::%s [%s] ERROR: Serious problem: not enough buffers left! Aborting acquisition!\n",
            driverName, functionName, portName);
        setIntegerParam(ADAcquire, 0);
        return(asynError);
    }
    pData = pImage_->GetData();
    if (pData) {
        memcpy(pRaw_->pData, pData, dataSize);
    } else {
        asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, 
            "%s::%s [%s] ERROR: pData is NULL!\n",
            driverName, functionName, portName);
        return asynError;
    }
    pImage_->Release();

    // Put the frame number into the buffer
    pRaw_->uniqueId = (int)pImage_->GetFrameID();
//    getIntegerParam(PGTimeStampMode, &timeStampMode);
    updateTimeStamp(&pRaw_->epicsTS);
    // Set the timestamps in the buffer
//    switch (timeStampMode) {
//        case TimeStampCamera:
//             pRaw_->timeStamp = timeStamp / 1e9;
//             break;
//        case TimeStampEPICS:
            pRaw_->timeStamp = pRaw_->epicsTS.secPastEpoch + pRaw_->epicsTS.nsec/1e9;
//            break;
//        case TimeStampHybrid:
//            // For now we just use EPICS time
//            pRaw_->timeStamp = pRaw_->epicsTS.secPastEpoch + pRaw_->epicsTS.nsec/1e9;
//            break;
//   }
    
    // Get any attributes that have been defined for this driver        
    getAttributes(pRaw_->pAttributeList);
    
    // Change the status to be readout...
    setIntegerParam(ADStatus, ADStatusReadout);
    callParamCallbacks();

    pRaw_->pAttributeList->add("ColorMode", "Color mode", NDAttrInt32, &colorMode);
    return status;
  }
    catch (Spinnaker::Exception &e)
    {
    	asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, 
    	    "%s::%s exception %s\n",
    	    driverName, functionName, e.what());
    	return asynError;
    }
}

asynStatus ADSpinnaker::getSPProperty(int paramIndex, void *pValue, bool setParam)
{
    static const char *functionName = "getSPProperty";
    propertyListElement* pElement = findProperty(paramIndex);
    if (pElement == 0) {
        asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
            "%s::%s unknown property index=%d\n", 
            driverName, functionName, paramIndex);
        return asynError;
    }
    SPPropertyType_t propertyType = pElement->propertyType_;
    const char *nodeName = pElement->nodeName_;
    

    try {
        CNodePtr pBase = (CNodePtr)pNodeMap_->GetNode(nodeName);
        if (!IsAvailable(pBase)) {
             asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, 
                "%s::%s Error: node %s is not available\n",
                driverName, functionName, nodeName);
             return asynError;
        }
        if (!IsReadable(pBase)) {
             asynPrint(pasynUserSelf, ASYN_TRACE_WARNING, 
                "%s::%s Warning: node %s is not readable\n",
                driverName, functionName, nodeName);
             return asynError;
        }
        switch (propertyType) {
            case SPPropertyTypeInt: {
                CIntegerPtr pNode = (CIntegerPtr)pBase;
                epicsInt32 value = (epicsInt32)pNode->GetValue();
                if (pValue) *(epicsInt32*)pValue = value;
                if (setParam) setIntegerParam(paramIndex, value);
                break;
            }
            case SPPropertyTypeBoolean: {
                CBooleanPtr pNode = (CBooleanPtr)pBase;
                epicsInt32 value = (epicsInt32)pNode->GetValue();
                if (pValue) *(epicsInt32*)pValue = value;
                if (setParam) setIntegerParam(paramIndex, value);
                break;
            }
            case SPPropertyTypeDouble: {
                CFloatPtr pNode = (CFloatPtr)pBase;
                epicsFloat64 value = (epicsFloat64)pNode->GetValue();
                if (pValue) *(epicsFloat64*)pValue = value;
                if (setParam) setDoubleParam(paramIndex, value);
                break;
            }
            case SPPropertyTypeEnum: {
                CEnumerationPtr pNode = (CEnumerationPtr)pBase;
                epicsInt32 value = (epicsInt32)pNode->GetIntValue();
                if (pValue) *(epicsInt32*)pValue = value;
                if (setParam) setIntegerParam(paramIndex, value);
                break;
            }
            case SPPropertyTypeString: {
                CStringPtr pNode = (CStringPtr)pBase;
                std::string value = epicsStrDup((pNode->GetValue()).c_str());
                if (pValue) *(std::string *)pValue = value;
                if (setParam) setStringParam(paramIndex, value);
                break;
            }
            case SPPropertyTypeCmd: {
                break;
            }
        }
    }
    catch (Spinnaker::Exception &e) {
        asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, 
            "%s::%s node %s exception %s\n",
            driverName, functionName, nodeName, e.what());
        return asynError;
    }
    return asynSuccess;
}


asynStatus ADSpinnaker::setSPProperty(int paramIndex, void *pValue, void *pReadbackValue, bool setParam)
{
    static const char *functionName = "setSPProperty";
    propertyListElement* pElement = findProperty(paramIndex);
    if (pElement == 0) {
        asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
            "%s::%s unknown property index=%d\n", 
            driverName, functionName, paramIndex);
        return asynError;
    }
    SPPropertyType_t propertyType = pElement->propertyType_;
    const char *nodeName = pElement->nodeName_;

    try {
        CNodePtr pBase = (CNodePtr)pNodeMap_->GetNode(nodeName);
        if (!IsAvailable(pBase)) {
             asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, 
                "%s::%s Error: node %s is not available\n",
                driverName, functionName, nodeName);
             return asynError;
        }
        if (!IsWritable(pBase)) {
             asynPrint(pasynUserSelf, ASYN_TRACE_WARNING, 
                "%s::%s Warning: node %s is not writable\n",
                driverName, functionName, nodeName);
             return asynError;
        }
        switch (propertyType) {
            case SPPropertyTypeInt: {
                CIntegerPtr pNode = (CIntegerPtr)pBase;
                epicsInt32 value;
                if (pValue)
                    value = *(epicsInt32*)pValue;
                else 
                    getIntegerParam(paramIndex, &value);
                // Check against the min and max
                int max = (int)pNode->GetMax();
                int min = (int)pNode->GetMin();
                int inc = (int)pNode->GetInc();
                if (inc != 1) {
                    value = (value/inc) * inc;
                }
                if (value < min) {
                   asynPrint(pasynUserSelf, ASYN_TRACE_WARNING, 
                        "%s::%s Warning: node %s value %d is less than minimum %d, setting to minimum\n",
                        driverName, functionName, nodeName, value, min);
                    value = min;
                }
                if (value > max) {
                   asynPrint(pasynUserSelf, ASYN_TRACE_WARNING, 
                        "%s::%s Warning: node %s value %d is greater than maximum %d, setting to maximum\n",
                        driverName, functionName, nodeName, value, max);
                    value = max;
                }
                pNode->SetValue(value);
                asynPrint(pasynUserSelf, ASYN_TRACEIO_DRIVER, 
                    "%s::%s set property %s to %d\n",
                    driverName, functionName, nodeName, value);
                if (IsReadable(pNode)) {
                    epicsInt32 readback = (epicsInt32)pNode->GetValue();
                    if (pReadbackValue) *(epicsInt32*)pReadbackValue = readback;
                    asynPrint(pasynUserSelf, ASYN_TRACEIO_DRIVER, 
                        "%s::%s readback property %s is %d\n",
                        driverName, functionName, nodeName, readback);
                    if (setParam) setIntegerParam(paramIndex, readback);
                }
                break;
            }
            case SPPropertyTypeBoolean: {
                CBooleanPtr pNode = (CBooleanPtr)pBase;
                epicsInt32 value;
                if (pValue) 
                    value = *(epicsInt32*)pValue;
                else
                    getIntegerParam(paramIndex, &value);
                *pNode = (bool)value;
                asynPrint(pasynUserSelf, ASYN_TRACEIO_DRIVER, 
                    "%s::%s set property %s to %d\n",
                    driverName, functionName, nodeName, value);
                if (IsReadable(pNode)) {
                    epicsInt32 readback = (epicsInt32)pNode->GetValue();
                    if (pReadbackValue) *(epicsInt32*)pReadbackValue = readback;
                    asynPrint(pasynUserSelf, ASYN_TRACEIO_DRIVER, 
                        "%s::%s readback property %s is %d\n",
                        driverName, functionName, nodeName, readback);
                    if (setParam) setIntegerParam(paramIndex, readback);
                }
                break;
            }
            case SPPropertyTypeDouble: {
                CFloatPtr pNode = (CFloatPtr)pBase;
                epicsFloat64 value;
                if (pValue) 
                    value = *(epicsFloat64*)pValue;
                else
                    getDoubleParam(paramIndex, &value);
                // Check against the min and max
                double max = pNode->GetMax();
                double min = pNode->GetMin();
                if (value < min) {
                   asynPrint(pasynUserSelf, ASYN_TRACE_WARNING, 
                        "%s::%s Warning: node %s value %f is less than minimum %f, setting to minimum\n",
                        driverName, functionName, nodeName, value, min);
                    value = min;
                }
                if (value > max) {
                   asynPrint(pasynUserSelf, ASYN_TRACE_WARNING, 
                        "%s::%s Warning: node %s value %f is greater than maximum %f, setting to maximum\n",
                        driverName, functionName, nodeName, value, max);
                    value = max;
                }
                pNode->SetValue(value);
                asynPrint(pasynUserSelf, ASYN_TRACEIO_DRIVER, 
                    "%s::%s set property %s to %f\n",
                    driverName, functionName, nodeName, value);
                if (IsReadable(pNode)) {
                    double readback = pNode->GetValue();
                    if (pReadbackValue) *(epicsFloat64*)pReadbackValue = readback;
                    asynPrint(pasynUserSelf, ASYN_TRACEIO_DRIVER, 
                        "%s::%s readback property %s is %f\n",
                        driverName, functionName, nodeName, readback);
                    if (setParam) setDoubleParam(paramIndex, readback);
                }
                break;
            }
            case SPPropertyTypeEnum: {
                CEnumerationPtr pNode = (CEnumerationPtr)pBase;
                epicsInt32 value;
                if (pValue) 
                    value = *(epicsInt32*)pValue;
                else
                    getIntegerParam(paramIndex, &value);
                pNode->SetIntValue(value);
                asynPrint(pasynUserSelf, ASYN_TRACEIO_DRIVER, 
                    "%s::%s set property %s to %d\n",
                    driverName, functionName, nodeName, value);
                if (IsReadable(pNode)) {
                    epicsInt32 readback = (epicsInt32)pNode->GetIntValue();
                    if (pReadbackValue) *(epicsInt32*)pReadbackValue = readback;
                    asynPrint(pasynUserSelf, ASYN_TRACEIO_DRIVER, 
                        "%s::%s readback property %s is %d\n",
                        driverName, functionName, nodeName, readback);
                    if (setParam) setIntegerParam(paramIndex, readback);
                }
                break;
            }
            case SPPropertyTypeString: {
                CStringPtr pNode = (CStringPtr)pBase;
                const char *value;
                if (pValue) 
                    value = (const char*)pValue;
                else {
                    std::string temp;
                    getStringParam(paramIndex, temp);
                    value = temp.c_str();
                }
                pNode->SetValue(value);
                asynPrint(pasynUserSelf, ASYN_TRACEIO_DRIVER, 
                    "%s::%s set property %s to %s\n",
                    driverName, functionName, nodeName, value);
                if (IsReadable(pNode)) {
                    std::string readback = epicsStrDup((pNode->GetValue()).c_str());
                    if (pReadbackValue) *(std::string*)pReadbackValue = readback;
                    asynPrint(pasynUserSelf, ASYN_TRACEIO_DRIVER, 
                        "%s::%s readback property %s is %s\n",
                        driverName, functionName, nodeName, readback.c_str());
                    if (setParam) setStringParam(paramIndex, readback);
                }
                break;
            }
            case SPPropertyTypeCmd: {
                CCommandPtr pNode = (CCommandPtr)pBase;
                pNode->Execute();
                asynPrint(pasynUserSelf, ASYN_TRACEIO_DRIVER, 
                    "%s::%s executed command %s\n",
                    driverName, functionName, nodeName);
                break;
            }
        }
    }
    catch (Spinnaker::Exception &e) {
        asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, 
            "%s::%s node %s exception %s\n",
            driverName, functionName, nodeName, e.what());
        return asynError;
    }
    return asynSuccess;
}


/** Sets an int32 parameter.
  * \param[in] pasynUser asynUser structure that contains the function code in pasynUser->reason. 
  * \param[in] value The value for this parameter 
  *
  * Takes action if the function code requires it.  ADAcquire, ADSizeX, and many other
  * function codes make calls to the Firewire library from this function. */

asynStatus ADSpinnaker::writeInt32( asynUser *pasynUser, epicsInt32 value)
{
    asynStatus status = asynSuccess;
    int function = pasynUser->reason;
    int addr;
//    PropertyType propType;
    static const char *functionName = "writeInt32";

    pasynManager->getAddr(pasynUser, &addr);
    if (addr < 0) addr=0;
//    propType = (PropertyType) addr;

    // Set the value in the parameter library.  This may change later but that's OK
    status = setIntegerParam(addr, function, value);

    if (function == ADAcquire) {
        if (value) {
            // start acquisition
            status = startCapture();
        } else {
            status = stopCapture();
        }

    } else if ( (function == ADSizeX)       ||
                (function == ADSizeY)       ||
                (function == ADMinX)        ||
                (function == ADMinY)        ||
                (function == ADBinX)        ||
                (function == ADBinY)        ||
                (function == ADImageMode)   ||
                (function == ADNumImages)   ||
                (function == NDDataType)) {    
        status = setImageParams();
    } else if ( (function == SPFrameRateEnable)   ||
                (function == SPPixelFormat)       ||
                (function == ADTriggerMode)       ||
                (function == SPTriggerSource)     ||
                (function == SPTriggerActivation) ||
                (function == SPSoftwareTrigger)) {
        status = setSPProperty(function, &value);
    } else if (function == ADReadStatus) {
        status = readStatus();
/*
    } else if ((function == ADTriggerMode)  || 
               (function == ADNumImages)    ||
               (function == ADNumExposures) ||
               (function == PGSkipFrames)) {
        status = setTrigger();
        

    } else if ((function == PGStrobeSource) || 
               (function == PGStrobeEnable) ||
               (function == PGStrobePolarity)) {
        status = setStrobe();
*/                
    } else {
        // If this parameter belongs to a base class call its method
        if (function < FIRST_SP_PARAM) status = ADDriver::writeInt32(pasynUser, value);
    }

    asynPrint(pasynUserSelf, ASYN_TRACEIO_DRIVER, 
        "%s::%s function=%d, addr=%d, value=%d, status=%d\n",
        driverName, functionName, function, addr, value, status);
            
    callParamCallbacks(addr);
    return status;
}

/** Sets an float64 parameter.
  * \param[in] pasynUser asynUser structure that contains the function code in pasynUser->reason. 
  * \param[in] value The value for this parameter 
  *
  * Takes action if the function code requires it.  The PGPropertyValueAbs
  * function code makes calls to the Firewire library from this function. */

asynStatus ADSpinnaker::writeFloat64( asynUser *pasynUser, epicsFloat64 value)
{
    asynStatus status = asynSuccess;
    int function = pasynUser->reason;
    epicsFloat64 readbackValue;
    int addr;
    static const char *functionName = "writeFloat64";
    
    pasynManager->getAddr(pasynUser, &addr);
    if (addr < 0) addr=0;

    // Set the value in the parameter library.  This may change later but that's OK
    status = setDoubleParam(addr, function, value);

    if (function == ADAcquireTime) {
        // Camera units are microseconds
        double tempValue = value * 1.e6;
        status = setSPProperty(ADAcquireTime, &tempValue, &readbackValue, false);
        setDoubleParam(ADAcquireTime, readbackValue/1.e6);
    
    } else if (function == SPTriggerDelay) {
        // Camera units are microseconds
        double tempValue = value * 1.e6;
        status = setSPProperty(SPTriggerDelay, &tempValue, &readbackValue, false);
        setDoubleParam(SPTriggerDelay, readbackValue/1.e6);
    
    } else if (function == ADGain) {
        status = setSPProperty(ADGain, &value);
            
    } else if (function == SPFrameRate) {
        status = setSPProperty(SPFrameRate, &value, &readbackValue);
        setDoubleParam(ADAcquirePeriod, 1./readbackValue);

    } else if (function == ADAcquirePeriod) {
        double tempValue = 1./value;
        status = setSPProperty(SPFrameRate, &tempValue, &readbackValue);
        setDoubleParam(ADAcquirePeriod, 1./readbackValue);
    
//    } else if ((function == PGStrobeDelay)  || 
//               (function == PGStrobeDuration)) {
//        status = setStrobe();
        
    } else {
        // If this parameter belongs to a base class call its method
        if (function < FIRST_SP_PARAM) status = ADDriver::writeFloat64(pasynUser, value);
    }

    asynPrint(pasynUser, ASYN_TRACEIO_DRIVER, 
        "%s::%s function=%d, addr=%d, value=%f, status=%d\n",
        driverName, functionName, function, addr, value, status);
    callParamCallbacks(addr);
    return status;
}

propertyListElement* ADSpinnaker::findProperty(int paramIndex)
{
    std::map<int, propertyListElement*>::iterator pEntry = propertyList_.find(paramIndex);
    if (pEntry == propertyList_.end()) {
        return 0;
    }
    propertyListElement* pElement = pEntry->second;
    return pElement;
}

asynStatus ADSpinnaker::readEnum(asynUser *pasynUser, char *strings[], int values[], int severities[], 
                               size_t nElements, size_t *nIn)
{
    int function = pasynUser->reason;
    int numEnums;
    int i;
    long long entryValue;
    static const char *functionName = "readEnum";

    propertyListElement *pElement = findProperty(function);

    if ((pElement == 0) || (pElement->propertyType_ != SPPropertyTypeEnum)) {
        return asynError;
    }
    const char *nodeName = pElement->nodeName_;

    *nIn = 0;
    
    try {
        CEnumerationPtr pNode = pNodeMap_->GetNode(nodeName);
        if (!IsAvailable(pNode) || !IsWritable(pNode)) {
            if (strings[0]) free(strings[0]);
            strings[0] = epicsStrDup("N.A.");
            values[0] = 0;
            *nIn = 1;
            return asynSuccess;
        }
        NodeList_t entries;
        pNode->GetEntries(entries);
        numEnums = (int)entries.size();

        for (i=0; ((i<numEnums) && (i<(int)nElements)); i++) {
            IEnumEntry *pEntry= dynamic_cast<IEnumEntry *>(entries[i]);
            const char *pString = epicsStrDup((pEntry->GetSymbolic()).c_str());
            if (IsAvailable(pEntry) && IsReadable(pEntry)) {
printf("%s:%s is available\n", nodeName, pString);               
                if (strings[*nIn]) free(strings[*nIn]);
                strings[*nIn] = epicsStrDup(pString);
                entryValue = pEntry->GetValue();
                values[*nIn] = (int)entryValue;
printf("%s:%s value=%lld, (int)value=%d\n", nodeName, pString, entryValue, values[*nIn]);               
                severities[*nIn] = 0;
                (*nIn)++;
            } else {
printf("%s:%s is not available\n", nodeName, pString);
            }
        }
    }

    catch (Spinnaker::Exception &e)
    {
    	asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, 
    	    "%s::%s node %s exception %s\n",
    	    driverName, functionName, nodeName, e.what());
    	return asynError;
    }
    return asynSuccess;   
}


asynStatus ADSpinnaker::setImageParams()
{
    //static const char *functionName = "setImageParams";
    
    //bool resumeAcquire;
    int imageMode;
    AcquisitionModeEnums acquisitionMode;

    if (!pCamera_) return asynError;

    // We translate between the areaDetector ImageMode and the Spinnaker AcquisitionMode
    getIntegerParam(ADImageMode, &imageMode);
    switch (imageMode) {
        case ADImageSingle:
            acquisitionMode = AcquisitionMode_SingleFrame;
            break;
        case ADImageMultiple:
            acquisitionMode = AcquisitionMode_MultiFrame;
            break;
        case ADImageContinuous:
            acquisitionMode = AcquisitionMode_Continuous;
            break;
          
    }
    
    setSPProperty(SPVideoMode);
    setSPProperty(SPAcquisitionMode, &acquisitionMode);
    setSPProperty(ADNumImages);
    setSPProperty(ADSizeX);
    setSPProperty(ADSizeY);
    setSPProperty(ADMinX);
    setSPProperty(ADMinY);
    setSPProperty(ADBinX);
    setSPProperty(ADBinY);

    // We read these back after setting all of them in case one setting affects another
    getSPProperty(SPVideoMode);
    getSPProperty(SPAcquisitionMode, &acquisitionMode);
    getSPProperty(ADNumImages);
    getSPProperty(ADSizeX);
    getSPProperty(ADSizeY);
    getSPProperty(ADMinX);
    getSPProperty(ADMinY);
    getSPProperty(ADBinX);
    getSPProperty(ADBinY);

    switch (acquisitionMode) {
        case AcquisitionMode_SingleFrame:
            imageMode = ADImageSingle;
            break;
        case AcquisitionMode_MultiFrame:
            imageMode = ADImageMultiple;
            break;
        case AcquisitionMode_Continuous:
            imageMode = ADImageContinuous;
            break;
        default:
            break;         
    }
    setIntegerParam(ADImageMode, imageMode);

/*
    // Must stop acquisition before changing the video mode
    asynPrint(pasynUserSelf, ASYN_TRACE_WARNING,
        "%s::%s calling Camera::StopCapture, pCamera_=%p\n",
        driverName, functionName, pCamera_);
    error = pCamera_->StopCapture();
    resumeAcquire = (error == PGRERROR_OK);
    getIntegerParam(PGPacketSize, &itemp);
    packetSize = itemp;
    setIntegerParam(PGMaxPacketSize, f7PacketInfo.maxBytesPerPacket);
    if (packetSize <= 0) {
        packetSize = f7PacketInfo.recommendedBytesPerPacket;
    } else {
        // Packet size must be a multiple of unitBytesPerPacket
        packetSize = (packetSize/f7PacketInfo.unitBytesPerPacket) * f7PacketInfo.unitBytesPerPacket;
        if (packetSize < pFormat7Info_->minPacketSize) packetSize = pFormat7Info_->minPacketSize;
        if (packetSize > f7PacketInfo.maxBytesPerPacket) packetSize = f7PacketInfo.maxBytesPerPacket;
    }
    asynPrint(pasynUserSelf, ASYN_TRACE_WARNING,
        "%s::%s calling Camera::SetFormat7Configuration\n"
        "  pCamera_=%p, &f7Settings=%p, packetSize=%d\n"
        "  f7Settings: mode=%d, offsetX=%d, offsetY=%d, width=%d, height=%d, pixelFormat=0x%x\n",
        driverName, functionName, pCamera_, &f7Settings, packetSize,
        f7Settings.mode, f7Settings.offsetX, f7Settings.offsetY, f7Settings.width, f7Settings.height, 
        f7Settings.pixelFormat);
    error = pCamera_->SetFormat7Configuration(&f7Settings, packetSize);
    checkError(error, functionName, "SetFormat7Configuration");
    if (resumeAcquire) {
        asynPrint(pasynUserSelf, ASYN_TRACE_WARNING,
            "%s::%s calling Camera::StartCapture, pCamera_=%p\n",
            driverName, functionName, pCamera_);
        error = pCamera_->StartCapture();
        checkError(error, functionName, "StartCapture");
    }

    // Read back the actual values
    error = pCamera_->GetFormat7Configuration(&f7Settings, &packetSizeActual, &percentage);
    if (checkError(error, functionName, "GetFormat7Configuration")) 
        return asynError;
    asynPrint(pasynUserSelf, ASYN_TRACE_WARNING,
        "%s::%s called Camera::GetFormat7Configuration\n"
        "  pCamera_=%p, &f7Settings=%p, packetSizeActual=%d, percentage=%f\n"
        "  f7Settings: mode=%d, offsetX=%d, offsetY=%d, width=%d, height=%d, pixelFormat=0x%x\n",
        driverName, functionName, pCamera_, &f7Settings, packetSizeActual, percentage,
        f7Settings.mode, f7Settings.offsetX, f7Settings.offsetY, f7Settings.width, f7Settings.height, 
        f7Settings.pixelFormat);
    setIntegerParam(ADMinX,        f7Settings.offsetX);
    setIntegerParam(ADMinY,        f7Settings.offsetY);
    setIntegerParam(ADSizeX,       f7Settings.width);
    setIntegerParam(ADSizeY,       f7Settings.height);
    setIntegerParam(PGPixelFormat, f7Settings.pixelFormat);
    setIntegerParam(PGPacketSizeActual, packetSizeActual);
    callParamCallbacks();
    
    // When the format7 mode changes the supported values of pixel format changes
    createDynamicEnums();
    // When the format7 mode changes the available properties can also change
    getAllProperties();
*/
    return asynSuccess;
}

/*
asynStatus ADSpinnaker::setStrobe()
{
    int polarity;
    int source;
    int enable;
    double delay;
    double duration;
    Error error;
    static const char *functionName = "setStrobe";

    getIntegerParam(PGStrobeSource,   &source);
    getIntegerParam(PGStrobeEnable,   &enable);
    getIntegerParam(PGStrobePolarity, &polarity);
    getDoubleParam(PGStrobeDelay,     &delay);
    getDoubleParam(PGStrobeDuration,  &duration);
    pStrobeControl_->source   = source;
    pStrobeControl_->onOff    = enable ? true : false;
    pStrobeControl_->polarity = polarity;
    pStrobeControl_->delay    = (float)(delay*1000.);
    pStrobeControl_->duration = (float)(duration*1000.);
    asynPrint(pasynUserSelf, ASYN_TRACE_WARNING,
        "%s::%s calling CameraBase::SetStrobe, pCameraBase_=%p, pStrobeControl_=%p\n"
        "  source=%d, onOff=%d, polarity=%d, delay=%f, duration=%f\n",
        driverName, functionName, pCameraBase_, pStrobeControl_,
        pStrobeControl_->source, pStrobeControl_->onOff, pStrobeControl_->polarity, 
        pStrobeControl_->delay, pStrobeControl_->duration);
    error = pCameraBase_->SetStrobe(pStrobeControl_);
    if (checkError(error, functionName, "SetStrobe")) 
        return asynError;
    return asynSuccess;
}
*/

/** Read all the propertyType settings and values from the camera.
 * This function will collect all the current values and settings from the camera,
 * and set the appropriate integer/double parameters in the param lib. If a certain propertyType
 * is not available in the given camera, this function will set all the parameters relating to that
 * propertyType to -1 or -1.0 to indicate it is not available.
 * Note the caller is responsible for calling any update callbacks if I/O interrupts
 * are to be processed after calling this function.
 * Returns asynStatus asynError or asynSuccess as an int.
 */
/*
asynStatus ADSpinnaker::getAllProperties()
{
    PropertyInfo *pPropInfo;
    Property *pProperty;
    Error error;
    int addr;
    double dtmp;
    static const char *functionName="getAllProperties";

    // Iterate through all of the available properties and update their values and settings 
    for (addr=0; addr<NUM_PROPERTIES; addr++) {
        pPropInfo = allPropInfos_[addr];
        pProperty = allProperties_[addr];
        asynPrint(pasynUserSelf, ASYN_TRACE_WARNING,
            "%s::%s calling CameraBase::GetPropertyInfo, pCameraBase_=%p, pPropInfo=%p, propertyType=%d\n",
            driverName, functionName, pCameraBase_, pPropInfo, pProperty->type);
        error = pCameraBase_->GetPropertyInfo(pPropInfo);
        if (checkError(error, functionName, "GetPropertyInfo")) 
            return asynError;
        error = pCameraBase_->GetProperty(pProperty);
        if (checkError(error, functionName, "GetProperty")) 
            return asynError;
        asynPrint(pasynUserSelf, ASYN_TRACE_WARNING,
            "%s::%s called CameraBase::GetProperty, pCameraBase_=%p, pProperty=%p, pProperty->valueA=%d\n",
            driverName, functionName, pCameraBase_, pProperty, pProperty->valueA);
        asynPrint(pasynUserSelf, ASYN_TRACE_FLOW,
            "%s:%s: checking propertyType %d\n",
            driverName, functionName, addr);

        if (pPropInfo->present) {
            setIntegerParam(addr, PGPropertyAvail,        1);
            setIntegerParam(addr, PGPropertyOnOffAvail,   pPropInfo->onOffSupported);
            setIntegerParam(addr, PGPropertyOnePushAvail, pPropInfo->onePushSupported);
            setIntegerParam(addr, PGPropertyAutoAvail,    pPropInfo->autoSupported);
            setIntegerParam(addr, PGPropertyManAvail,     pPropInfo->manualSupported);
            setIntegerParam(addr, PGPropertyAbsAvail,     pPropInfo->absValSupported);
            setIntegerParam(addr, PGPropertyOnOff,        pProperty->onOff);
            setIntegerParam(addr, PGPropertyAutoMode,     pProperty->autoManualMode);
            setIntegerParam(addr, PGPropertyAbsMode,      pProperty->absControl);
            setIntegerParam(addr, PGPropertyValue,        pProperty->valueA);
            setIntegerParam(addr, PGPropertyValueB,       pProperty->valueB);
            setIntegerParam(addr, PGPropertyValueMin,     pPropInfo->min);
            setIntegerParam(addr, PGPropertyValueMax,     pPropInfo->max);
        } else {
            // If the propertyType is not available in the camera, we just set
             * all the parameters to 0 or -1 to indicate this is not available to the user.
            setIntegerParam(addr, PGPropertyAvail,        0);
            setIntegerParam(addr, PGPropertyOnOffAvail,   0);
            setIntegerParam(addr, PGPropertyOnOff,        0);
            setIntegerParam(addr, PGPropertyOnePushAvail, 0);
            setIntegerParam(addr, PGPropertyAutoAvail,    0);
            setIntegerParam(addr, PGPropertyManAvail,     0);
            setIntegerParam(addr, PGPropertyAbsAvail,     0);
            setIntegerParam(addr, PGPropertyAutoMode,     0);
            setIntegerParam(addr, PGPropertyAbsMode,      -1);
            setIntegerParam(addr, PGPropertyValue,        -1);
            setIntegerParam(addr, PGPropertyValueMin,     -1);
            setIntegerParam(addr, PGPropertyValueMax,     -1);
        }

        // If the propertyType does not support 'absolute' control then we just
         * set all the absolute values to -1.0 to indicate it is not available to the user
        if (pPropInfo->absValSupported) { 
            setDoubleParam(addr,  PGPropertyValueAbs,    pProperty->absValue);
            setDoubleParam(addr,  PGPropertyValueAbsMin, pPropInfo->absMin);
            setDoubleParam(addr,  PGPropertyValueAbsMax, pPropInfo->absMax);
        } else {
            setDoubleParam(addr,  PGPropertyValueAbs,    -1.0);
            setDoubleParam(addr,  PGPropertyValueAbsMax, -1.0);
            setDoubleParam(addr,  PGPropertyValueAbsMin, -1.0);
        }
    }

    // Map a few of the AreaDetector parameters on to the camera properties
    getDoubleParam(SHUTTER, PGPropertyValueAbs, &dtmp);
    // Camera units are ms
    setDoubleParam(ADAcquireTime, dtmp/1000.);

    getDoubleParam(FRAME_RATE, PGPropertyValueAbs, &dtmp);
    // Camera units are fps
    setDoubleParam(ADAcquirePeriod, 1./dtmp);

    getDoubleParam(GAIN, PGPropertyValueAbs, &dtmp);
    setDoubleParam(ADGain, dtmp);

    // Do callbacks for each propertyType
    for (addr=0; addr<NUM_PROPERTIES; addr++) callParamCallbacks(addr);

    return asynSuccess;
}

*/

asynStatus ADSpinnaker::startCapture()
{
    static const char *functionName = "startCapture";

    // Start the camera transmission...
    setIntegerParam(ADNumImagesCounter, 0);
    setShutter(1);
printf("%s::%s calling BeginAcquisition()\n", driverName, functionName);
    try {
        pCamera_->BeginAcquisition();
        epicsEventSignal(startEventId_);
    }
    catch (Spinnaker::Exception &e)
    {
    	asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, 
    	    "%s::%s exception %s\n",
    	    driverName, functionName, e.what());
    	return asynError;
    }
    return asynSuccess;
}


asynStatus ADSpinnaker::stopCapture()
{
    int status;
    static const char *functionName = "stopCapture";

    setIntegerParam(ADAcquire, 0);
    setShutter(0);
    // Need to wait for the task to set the status to idle
    while (1) {
        getIntegerParam(ADStatus, &status);
        if (status == ADStatusIdle) break;
printf("Waiting for ADStatusIdle\n");
        unlock();
        epicsThreadSleep(.1);
        lock();
    }
    printf("%s::%s calling EndAcquisition()\n", driverName, functionName);
    try {
        pCamera_->EndAcquisition();
    }
    catch (Spinnaker::Exception &e)
    {
    	asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, 
    	    "%s::%s exception %s\n",
    	    driverName, functionName, e.what());
    	return asynError;
    }
    return asynSuccess;
}


asynStatus ADSpinnaker::readStatus()
{
    //static const char *functionName = "readStatus";

    getSPProperty(ADTemperatureActual);
 
 /*
    error = pCameraBase_->GetStats(pCameraStats_);
    if (checkError(error, functionName, "GetStats")) 
        return asynError;
    asynPrint(pasynUserSelf, ASYN_TRACE_WARNING,
        "%s::%s calling CameraBase::GetStats, pCameraBase_=%p, pCameraStats_=%p, pCameraStats_->temperature=%d\n",
        driverName, functionName, pCameraBase_, pCameraStats_, pCameraStats_->temperature);
    setIntegerParam(PGCorruptFrames,    pCameraStats_->imageCorrupt);
    setIntegerParam(PGDriverDropped,    pCameraStats_->imageDriverDropped);
    if (pCameraStats_->imageXmitFailed == 0x80000000) pCameraStats_->imageXmitFailed = 0;
    setIntegerParam(PGTransmitFailed,   pCameraStats_->imageXmitFailed);
    setIntegerParam(PGDroppedFrames,    pCameraStats_->imageDropped);
*/    
    callParamCallbacks();
    return asynSuccess;
}


/** Print out a report; calls ADDriver::report to get base class report as well.
  * \param[in] fp File pointer to write output to
  * \param[in] details Level of detail desired.  If >1 prints information about 
               supported video formats and modes, etc.
 */
/*
void ADSpinnaker::report(FILE *fp, int details)
{
    unsigned int numCameras;
    Error error;
    Camera cam;
    int mode, rate;
    int source;
    unsigned int i, j;
    asynStatus status;
    bool supported;
    int property;
    int pixelFormatIndex;
    static const char *functionName = "report";
    
    error = pBusMgr_->GetNumOfCameras(&numCameras);
    if (checkError(error, functionName, "GetNumOfCameras")) return;

    fprintf(fp, "\nNumber of cameras detected: %u\n", numCameras);

    for (i=0; i<numCameras; i++) {
        PGRGuid guid;
        CameraInfo camInfo;
        error = pBusMgr_->GetCameraFromIndex(i, &guid);
        if (checkError(error, functionName, "GetCameraFromIndex")) return;

        // Connect to camera
        error = cam.Connect(&guid);
        if (checkError(error, functionName, "Connect")) return;

        // Get the camera information
        error = cam.GetCameraInfo(&camInfo);
        if (checkError(error, functionName, "GetCameraInfo")) return;

        fprintf(fp, "\n");
        fprintf(fp, "Serial number:       %u\n", camInfo.serialNumber);
        fprintf(fp, "Camera model:        %s\n", camInfo.modelName);
        fprintf(fp, "Camera vendor:       %s\n", camInfo.vendorName);
        fprintf(fp, "Sensor:              %s\n", camInfo.sensorInfo);
        fprintf(fp, "Resolution:          %s\n", camInfo.sensorResolution);
        fprintf(fp, "Firmware version:    %s\n", camInfo.firmwareVersion);
        fprintf(fp, "Firmware build time: %s\n", camInfo.firmwareBuildTime);

        // Disconnect from camera
        error = cam.Disconnect();
        if (checkError(error, functionName, "Connect")) return;
    }
    
    // It seems like disconnected from the cam object here causes the already found camera to
    // need to be reconnected
    status = connectCamera();
    if (status) {
        asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
            "%s::%s error calling connectCamera()\n",
            driverName, functionName);
    }
    
    if (pCamera_) {
        getIntegerParam(PGFormat7Mode, &mode);
        pFormat7Info_->mode = (Mode)mode;
        error = pCamera_->GetFormat7Info(pFormat7Info_, &supported);
        if (checkError(error, functionName, "GetFormat7Info")) 
            return;
        fprintf(fp, "\n");
        fprintf(fp, "Currently connected IIDC camera Format7 information\n");
        fprintf(fp, "  mode:            %d\n", pFormat7Info_->mode);
        fprintf(fp, "  maxWidth:        %d\n", pFormat7Info_->maxWidth);
        fprintf(fp, "  maxHeight:       %d\n", pFormat7Info_->maxHeight);
        fprintf(fp, "  offsetHStepSize: %d\n", pFormat7Info_->offsetHStepSize);
        fprintf(fp, "  offsetVStepSize: %d\n", pFormat7Info_->offsetVStepSize);
        fprintf(fp, "  imageHStepSize:  %d\n", pFormat7Info_->imageHStepSize);
        fprintf(fp, "  imageVStepSize:  %d\n", pFormat7Info_->imageVStepSize);
        fprintf(fp, "  pixelFormatBitField       0x%x\n", pFormat7Info_->pixelFormatBitField);
        fprintf(fp, "  vendorPixelFormatBitField 0x%x\n", pFormat7Info_->vendorPixelFormatBitField);
                    
    } else if (pGigECamera_) {
        error = pGigECamera_->GetGigEImageSettingsInfo(pGigEImageSettingsInfo_);
        if (checkError(error, functionName, "GetGigEImageSettingsInfo")) 
            return;
        fprintf(fp, "\n");
        fprintf(fp, "Currently connected GigE camera image information\n");
        fprintf(fp, "  maxWidth:        %d\n", pGigEImageSettingsInfo_->maxWidth);
        fprintf(fp, "  maxHeight:       %d\n", pGigEImageSettingsInfo_->maxHeight);
        fprintf(fp, "  offsetHStepSize: %d\n", pGigEImageSettingsInfo_->offsetHStepSize);
        fprintf(fp, "  offsetVStepSize: %d\n", pGigEImageSettingsInfo_->offsetVStepSize);
        fprintf(fp, "  imageHStepSize:  %d\n", pGigEImageSettingsInfo_->imageHStepSize);
        fprintf(fp, "  imageVStepSize:  %d\n", pGigEImageSettingsInfo_->imageVStepSize);
        fprintf(fp, "  pixelFormatBitField       0x%x\n", pGigEImageSettingsInfo_->pixelFormatBitField);
        fprintf(fp, "  vendorPixelFormatBitField 0x%x\n", pGigEImageSettingsInfo_->vendorPixelFormatBitField);
    }

    if (details < 1) goto done;
    
    if (pCamera_) {
        VideoMode videoMode;
        FrameRate frameRate;
        unsigned int packetSize;
        float percentage;
        Format7ImageSettings f7Settings;
        fprintf(fp, "\nSupported IIDC video modes and rates:\n");
        for (mode=0; mode<NUM_VIDEOMODES; mode++) {
            videoMode = (VideoMode)mode;
            if (videoMode == VIDEOMODE_FORMAT7) continue;
            for (rate=0; rate<NUM_FRAMERATES; rate++) {
                frameRate = (FrameRate)rate;
                if (frameRate == FRAMERATE_FORMAT7) continue;
                error = pCamera_->GetVideoModeAndFrameRateInfo(videoMode, frameRate, &supported);
                if (checkError(error, functionName, "GetVideoModeAndFrameRateInfo")) 
                    return;
                if (supported) {
                    fprintf(fp, "    Video mode %d (%s) and frame rate %d (%s)\n", 
                    mode, videoModeStrings[mode], rate, frameRateStrings[rate]);
                }
            }
        }
        error = pCamera_->GetVideoModeAndFrameRate(&videoMode, &frameRate);
        if (checkError(error, functionName, "GetVideoModeAndFrameRate")) 
            return;
        fprintf(fp, "\nCurrent image settings\n");
        fprintf(fp, "  Mode: %d (%s)\n", videoMode, videoModeStrings[videoMode]);
        fprintf(fp, "  Rate: %d (%s)\n", frameRate, frameRateStrings[frameRate]);
        if (videoMode == VIDEOMODE_FORMAT7) {
            error = pCamera_->GetFormat7Configuration(&f7Settings, &packetSize, &percentage);
            if (checkError(error, functionName, "GetFormat7Configuration")) 
                return;
            pixelFormatIndex = getPixelFormatIndex(f7Settings.pixelFormat);     
            fprintf(fp, "  Format7 video format currently selected\n");
            fprintf(fp, "    Packet size: %d\n",    packetSize);
            fprintf(fp, "    Bandwidth %%: %f\n",   percentage);
            fprintf(fp, "           Mode: %d\n",    f7Settings.mode);
            fprintf(fp, "         Offset: %d %d\n", f7Settings.offsetX, f7Settings.offsetY);
            fprintf(fp, "           Size: %d %d\n", f7Settings.width, f7Settings.height);
            fprintf(fp, "    PixelFormat: index=%d, value=0x%x [%s]\n",
                             pixelFormatIndex, f7Settings.pixelFormat, pixelFormatStrings[pixelFormatIndex]);
        }

    } else if (pGigECamera_) {
        int packetSize;
        int packetDelay;
        int heartBeat;
        int heartBeatTimeout;
        unsigned int binX, binY;
        GigEImageSettings gigESettings;
        Mode gigEMode;
        error = pGigECamera_->GetGigEImagingMode(&gigEMode);
        if (checkError(error, functionName, "GetGigEImagingMode")) 
            return;
        fprintf(fp, "\nSupported GigE modes:\n");
        for (mode=0; mode<NUM_MODES; mode++) {
            error = pGigECamera_->QueryGigEImagingMode((Mode)mode, &supported);
            if (checkError(error, functionName, "QueryGigEImagingMode")) 
                return;
            if (supported) {
                fprintf(fp, "    GigE mode %d\n", mode);
                asynPrint(pasynUserSelf, ASYN_TRACE_WARNING,
                    "%s::%s calling GigECamera::SetGigEImagingMode, pGigECamera_=%p, mode=%d\n",
                    driverName, functionName, pGigECamera_, mode);
                error = pGigECamera_->SetGigEImagingMode((Mode)mode);
                if (checkError(error, functionName, "SetGigEImagingMode"))
                    return;
                asynPrint(pasynUserSelf, ASYN_TRACE_WARNING,
                    "%s::%s calling GigECamera::GetGigEImageSettingsInfo, pGigECamera_=%p, pGigEImageSettingsInfo_=%p\n",
                    driverName, functionName, pGigECamera_, pGigEImageSettingsInfo_);
                error = pGigECamera_->GetGigEImageSettingsInfo(pGigEImageSettingsInfo_);
                if (checkError(error, functionName, "GetGigEImageSettingsInfo")) 
                    return;
                fprintf(fp, "     maxWidth:        %d\n", pGigEImageSettingsInfo_->maxWidth);
                fprintf(fp, "     maxHeight:       %d\n", pGigEImageSettingsInfo_->maxHeight);
                fprintf(fp, "     offsetHStepSize: %d\n", pGigEImageSettingsInfo_->offsetHStepSize);
                fprintf(fp, "     offsetVStepSize: %d\n", pGigEImageSettingsInfo_->offsetVStepSize);
                fprintf(fp, "     imageHStepSize:  %d\n", pGigEImageSettingsInfo_->imageHStepSize);
                fprintf(fp, "     imageVStepSize:  %d\n", pGigEImageSettingsInfo_->imageVStepSize);
                fprintf(fp, "     pixelFormatBitField       0x%x\n", pGigEImageSettingsInfo_->pixelFormatBitField);
                fprintf(fp, "     vendorPixelFormatBitField 0x%x\n", pGigEImageSettingsInfo_->vendorPixelFormatBitField);
            }    
        }
        error = pGigECamera_->SetGigEImagingMode(gigEMode);
        if (checkError(error, functionName, "SetGigEImagingMode")) 
            return;
        error = pGigECamera_->GetGigEImageSettings(&gigESettings);
        if (checkError(error, functionName, "GetGigEImageSettings")) 
            return;
        error = pGigECamera_->GetGigEImageBinningSettings(&binX, &binY);
        if (checkError(error, functionName, "GetGigEImageBinningSettings")) 
            return;
        pixelFormatIndex = getPixelFormatIndex(gigESettings.pixelFormat);
        getAllGigEProperties();
        getIntegerParam(PACKET_SIZE,       PGGigEPropertyValue, &packetSize);
        getIntegerParam(PACKET_DELAY,      PGGigEPropertyValue, &packetDelay);
        getIntegerParam(HEARTBEAT,         PGGigEPropertyValue, &heartBeat);
        getIntegerParam(HEARTBEAT_TIMEOUT, PGGigEPropertyValue, &heartBeatTimeout);
        fprintf(fp, "\n");
        fprintf(fp, "Current GigE image settings\n");
        fprintf(fp, "               Mode: %d\n",    gigEMode);
        fprintf(fp, "        Packet size: %d\n",    packetSize);
        fprintf(fp, "       Packet delay: %d\n",    packetDelay);
        fprintf(fp, "          Heartbeat: %d\n",    heartBeat);
        fprintf(fp, "  Heartbeat timeout: %d\n",    heartBeatTimeout);
        fprintf(fp, "             Offset: %d %d\n", gigESettings.offsetX, gigESettings.offsetY);
        fprintf(fp, "               Size: %d %d\n", gigESettings.width, gigESettings.height);
        fprintf(fp, "            Binning: %u %u\n", binX, binY);
        fprintf(fp, "        PixelFormat: index=%d, value=0x%x [%s]\n",
                             pixelFormatIndex, gigESettings.pixelFormat, pixelFormatStrings[pixelFormatIndex]);
    }

    // Iterate through all of the available properties and report on them 
    fprintf(fp, "\nSupported properties\n");
    Property *pProperty;
    PropertyInfo *pPropInfo;
    for (property=0; property<NUM_PROPERTIES; property++) {
        pProperty = allProperties_[property];
        pPropInfo = allPropInfos_[property];
        error = pCameraBase_->GetProperty(pProperty);
        if (checkError(error, functionName, "GetProperty")) 
            return;
        error = pCameraBase_->GetPropertyInfo(pPropInfo);
        if (checkError(error, functionName, "GetPropertyInfo")) 
            return;
        if (pProperty->present) {
            fprintf(fp, "Property %s \n",                       propertyTypeStrings[property]);
            fprintf(fp, "  min           = %d\n",               pPropInfo->min);
            fprintf(fp, "  max           = %d\n",               pPropInfo->max);
            fprintf(fp, "  value         = %d\n",               pProperty->valueA);
            fprintf(fp, "  hasAutoMode   = %d     status=%d\n", pPropInfo->autoSupported,    pProperty->autoManualMode);
            fprintf(fp, "  hasManualMode = %d     status=%d\n", pPropInfo->manualSupported,  !pProperty->autoManualMode);
            fprintf(fp, "  hasOnOff      = %d     status=%d\n", pPropInfo->onOffSupported,   pProperty->onOff);
            fprintf(fp, "  hasOnePush    = %d     status=%d\n", pPropInfo->onePushSupported, pProperty->onePush);
            fprintf(fp, "  hasReadout    = %d\n",               pPropInfo->readOutSupported);
            fprintf(fp, "  hasAbsControl = %d     status=%d\n", pPropInfo->absValSupported,  pProperty->absControl);
            if (pPropInfo->absValSupported) { 
                fprintf(fp, "    units        = %s    abbreviated=%s\n", pPropInfo->pUnits, pPropInfo->pUnitAbbr);
                fprintf(fp, "    min          = %f\n", pPropInfo->absMin);
                fprintf(fp, "    max          = %f\n", pPropInfo->absMax);
                fprintf(fp, "    value        = %f\n", pProperty->absValue);
            }
        }
        else {
            fprintf(fp, "Property %s is not supported\n", propertyTypeStrings[property]);
        }

    }

    error = pCameraBase_->GetTriggerMode(pTriggerMode_);
    if (checkError(error, functionName, "GetTriggerMode")) 
        return;
    fprintf(fp, "\nTrigger mode\n");
    fprintf(fp, "       Mode: %d\n", pTriggerMode_->mode);
    fprintf(fp, "      onOff: %d\n", pTriggerMode_->onOff);
    fprintf(fp, "   polarity: %d\n", pTriggerMode_->polarity);
    fprintf(fp, "     source: %d\n", pTriggerMode_->source);
    fprintf(fp, "  parameter: %d\n", pTriggerMode_->parameter);

    error = pCameraBase_->GetTriggerModeInfo(pTriggerModeInfo_);
    if (checkError(error, functionName, "GetTriggerModeInfo")) 
        return;
    fprintf(fp, "\nTrigger mode information\n");
    fprintf(fp, "                   present: %d\n",   pTriggerModeInfo_->present);
    fprintf(fp, "          readOutSupported: %d\n",   pTriggerModeInfo_->readOutSupported);
    fprintf(fp, "            onOffSupported: %d\n",   pTriggerModeInfo_->onOffSupported);
    fprintf(fp, "         polaritySupported: %d\n",   pTriggerModeInfo_->polaritySupported);
    fprintf(fp, "             valueReadable: %d\n",   pTriggerModeInfo_->valueReadable);
    fprintf(fp, "                sourceMask: 0x%x\n", pTriggerModeInfo_->sourceMask);
    fprintf(fp, "  softwareTriggerSupported: %d\n",   pTriggerModeInfo_->softwareTriggerSupported);
    fprintf(fp, "                  modeMask: 0x%x\n", pTriggerModeInfo_->modeMask);

    fprintf(fp, "\nStrobe information\n");
    source = pStrobeControl_->source;
    for (j=0; j<NUM_GPIO_PINS; j++) {
        pStrobeInfo_->source = j;
        error = pCameraBase_->GetStrobeInfo(pStrobeInfo_);
        if (checkError(error, functionName, "GetStrobeInfo")) 
            return;
        if (pStrobeInfo_->present) {
            fprintf(fp, "     Strobe source %d: present\n", j);
            fprintf(fp, "     readOutSupported: %d\n", pStrobeInfo_->readOutSupported);
            fprintf(fp, "       onOffSupported: %d\n", pStrobeInfo_->onOffSupported);
            fprintf(fp, "    polaritySupported: %d\n", pStrobeInfo_->polaritySupported);
            fprintf(fp, "             minValue: %f\n", pStrobeInfo_->minValue);
            fprintf(fp, "             maxValue: %f\n", pStrobeInfo_->maxValue);
            pStrobeControl_->source = j;
            error = pCameraBase_->GetStrobe(pStrobeControl_);
            if (checkError(error, functionName, "GetStrobe")) 
                return;
            fprintf(fp, "                onOff: %d\n", pStrobeControl_->onOff);
            fprintf(fp, "             polarity: %u\n", pStrobeControl_->polarity);
            fprintf(fp, "                delay: %f\n", pStrobeControl_->delay);
            fprintf(fp, "             duration: %f\n", pStrobeControl_->duration);
        }
    }
    pStrobeControl_->source = source;


    error = pCameraBase_->GetStats(pCameraStats_);
    if (checkError(error, functionName, "GetStats")) 
        return;
    if (pCameraStats_->imageXmitFailed == 0x80000000) pCameraStats_->imageXmitFailed = 0;
    fprintf(fp, "\nCamera statistics\n");
    fprintf(fp, "              Images dropped: %u\n", pCameraStats_->imageDropped);
    fprintf(fp, "              Images corrupt: %u\n", pCameraStats_->imageCorrupt);
    fprintf(fp, "             Transmit failed: %u\n", pCameraStats_->imageXmitFailed);
    fprintf(fp, "              Driver dropped: %u\n", pCameraStats_->imageDriverDropped);
    fprintf(fp, "        Register read failed: %u\n", pCameraStats_->regReadFailed);
    fprintf(fp, "       Register write failed: %u\n", pCameraStats_->regWriteFailed);
    fprintf(fp, "                 Port errors: %u\n", pCameraStats_->portErrors);
    fprintf(fp, "             Camera power up: %d\n", pCameraStats_->cameraPowerUp);
    fprintf(fp, "                  # voltages: %d\n", pCameraStats_->numVoltages);
    for (j=0; j<pCameraStats_->numVoltages; j++) {
        fprintf(fp, "                   voltage %d: %f\n", j, pCameraStats_->cameraVoltages[j]);
    }
    fprintf(fp, "                  # currents: %d\n", pCameraStats_->numCurrents);
    for (j=0; j<pCameraStats_->numCurrents; j++) {
        fprintf(fp, "                   current %d: %f\n", j, pCameraStats_->cameraCurrents[j]);
    }
    fprintf(fp, "             Temperature (C): %f\n", pCameraStats_->temperature/10. - 273.15);
    fprintf(fp, "   Time since initialization: %u\n", pCameraStats_->timeSinceInitialization);
    fprintf(fp, "        Time since bus reset: %u\n", pCameraStats_->timeSinceBusReset);
    fprintf(fp, "  # resend packets requested: %u\n", pCameraStats_->numResendPacketsRequested);
    fprintf(fp, "   # resend packets received: %u\n", pCameraStats_->numResendPacketsReceived);
    fprintf(fp, "                  Time stamp: %f\n", pCameraStats_->timeStamp.seconds + 
                                                      pCameraStats_->timeStamp.microSeconds/1e6);
    done:          
    ADDriver::report(fp, details);
    return;
}

*/
static const iocshArg configArg0 = {"Port name", iocshArgString};
static const iocshArg configArg1 = {"cameraId", iocshArgInt};
static const iocshArg configArg2 = {"traceMask", iocshArgInt};
static const iocshArg configArg3 = {"memoryChannel", iocshArgInt};
static const iocshArg configArg4 = {"maxBuffers", iocshArgInt};
static const iocshArg configArg5 = {"maxMemory", iocshArgInt};
static const iocshArg configArg6 = {"priority", iocshArgInt};
static const iocshArg configArg7 = {"stackSize", iocshArgInt};
static const iocshArg * const configArgs[] = {&configArg0,
                                              &configArg1,
                                              &configArg2,
                                              &configArg3,
                                              &configArg4,
                                              &configArg5,
                                              &configArg6,
                                              &configArg7};
static const iocshFuncDef configADSpinnaker = {"ADSpinnakerConfig", 8, configArgs};
static void configCallFunc(const iocshArgBuf *args)
{
    ADSpinnakerConfig(args[0].sval, args[1].ival, args[2].ival, 
                    args[3].ival, args[4].ival, args[5].ival,
                    args[6].ival, args[7].ival);
}


static void ADSpinnakerRegister(void)
{
    iocshRegister(&configADSpinnaker, configCallFunc);
}

extern "C" {
epicsExportRegistrar(ADSpinnakerRegister);
}
