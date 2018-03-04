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

/*
#define PGPacketSizeString            "PG_PACKET_SIZE"
#define PGPacketSizeActualString      "PG_PACKET_SIZE_ACTUAL"
#define PGMaxPacketSizeString         "PG_MAX_PACKET_SIZE"
#define PGPacketDelayString           "PG_PACKET_DELAY"
#define PGPacketDelayActualString     "PG_PACKET_DELAY_ACTUAL"
#define PGBandwidthString             "PG_BANDWIDTH"
#define PGTimeStampModeString         "PG_TIME_STAMP_MODE"
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
    SPPropertyTypeDoubleMin,
    SPPropertyTypeDoubleMax,
    SPPropertyTypeString,
    SPPropertyTypeCmd,
    SPPropertyTypeUnknown
} SPPropertyType_t;

typedef enum {
    SPPixelConvertNone,
    SPPixelConvertMono8,
    SPPixelConvertMono16,
    SPPixelConvertRaw16,
    SPPixelConvertRGB8,
    SPPixelConvertRGB16
} SPPixelConvert_t;


/*
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

typedef enum {
    SPConvertToEPICS,
    SPConvertFromEPICS
} SPConvertDirection_t;


class ImageEventHandler : public ImageEvent
{
public:

    ImageEventHandler(epicsMessageQueue *pMsgQ) 
     : pMsgQ_(pMsgQ)
    {}
    ~ImageEventHandler() {}
  
    void OnImageEvent(ImagePtr image) {
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
    void report(FILE *fp, int details);
    /**< These should be private but are called from C callback functions, must be public. */
    void imageGrabTask();
    void shutdown();

protected:
    int SPVideoMode;            // 0
#define FIRST_SP_PARAM SPVideoMode
    int SPFrameRate;            // 1 
    int SPFrameRateAuto;        // 2
    int SPFrameRateEnable;      // 3
    int SPExposureAuto;         // 5
    int SPGainAuto;             // 7
    int SPBlackLevel;           // 8
    int SPBlackLevelAuto;       // 9
    int SPBlackLevelBalanceAuto;// 10
    int SPSaturation;           // 11
    int SPSaturationEnable;     // 12
    int SPGamma;                // 13
    int SPGammaEnable;          // 14
    int SPSharpening;           // 15
    int SPSharpeningAuto;       // 16
    int SPSharpeningEnable;     // 17
    int SPPixelFormat;          // 18
    int SPConvertPixelFormat;   // 19
    int SPTriggerSource;        // 20
    int SPTriggerActivation;    // 21
    int SPTriggerDelay;         // 22
    int SPSoftwareTrigger;      // 23
    int SPBalanceRatio;         // 24
    int SPBalanceRatioSelector; // 25
    int SPBalanceWhiteAuto;     // 26
    int SPTransmitFailureCount; // 27
    int SPBufferUnderrunCount;  // 28
    int SPFailedBufferCount;    // 29
    int SPFailedPacketCount;    // 30

//    int PGPacketSize;             /** Size of data packets from camera                (int32 write/read) */
//    int PGPacketSizeActual;       /** Size of data packets from camera                (int32 write/read) */
//    int PGMaxPacketSize;          /** Maximum size of data packets from camera        (int32 write/read) */
//    int PGPacketDelay;            /** Packet delay in usec from camera, GigE only     (int32 write/read) */
//    int PGPacketDelayActual;      /** Packet delay in usec from camera, GigE only     (int32 read) */
//    int PGBandwidth;              /** Bandwidth in MB/s                               (float64 read) */
//    int PGTimeStampMode;          /** Time stamp mode (PGTimeStamp_t)                 (int32 write/read) */

private:
    class SPProperty {
        public:
            SPProperty(ADSpinnaker *pDrvIn, int asynParamIn, const char *nodeNameIn, SPPropertyType_t propertyType=SPPropertyTypeUnknown);
            ADSpinnaker *pDrv;
            SPPropertyType_t propertyType;
            int asynParam;
            gcstring nodeName;
            CNodePtr pBase;
            bool isImplemented;
            asynStatus setValue(void *value=0, void *readbackValue=0, bool setParam=true);
            asynStatus getValue(void *value=0, bool setParam=true);
            asynStatus update();
            double convertUnits(double inputValue, SPConvertDirection_t direction);
            int convertUnits(int inputValue, SPConvertDirection_t direction);
    };

    /* Local methods to this class */
    asynStatus grabImage();
    asynStatus startCapture();
    asynStatus stopCapture();

    asynStatus connectCamera();
    asynStatus disconnectCamera();
    asynStatus readStatus();

    SPProperty* findProperty(int asynParam);

    /* camera property control functions */
    asynStatus createSPProperty(int *pAsynParam, asynParamType paramType, const char *asynDrvUser, const char *nodeName);
    asynStatus createSPProperty(int asynParam, const char *nodeName);
    asynStatus setSPProperty(int asynParam, void *value=0, void *readbackValue=0, bool setParam=true);
    asynStatus getSPProperty(int asynParam, void *value=0, bool setParam=true);
    asynStatus updateSPProperties();

    asynStatus setImageParams();
    
    void imageEventCallback(ImagePtr pImage);
    gcstring getValueAsString(INodeMap *pNodeMap, gcstring nodeName, gcstring & displayName);
    void reportNode(FILE *fp, INodeMap *pNodeMap, gcstring nodeName, int level);

    /* Data */
    int cameraId_;
    int memoryChannel_;
    
    INodeMap *pNodeMap_;    
    SystemPtr system_;
    CameraList camList_;
    CameraPtr pCamera_;
    ImagePtr pImage_;
    ImageEventHandler *pImageEventHandler_;
    std::map<int, SPProperty*> propertyList_;

    int exiting_;
    epicsEventId startEventId_;
    epicsMessageQueue *pCallbackMsgQ_;
    NDArray *pRaw_;
};

ADSpinnaker::SPProperty::SPProperty(ADSpinnaker *pDrvIn, int asynParamIn, const char *nodeNameIn, SPPropertyType_t propertyTypeIn)

    : pDrv(pDrvIn),
      propertyType(propertyTypeIn),
      asynParam(asynParamIn),
      nodeName(nodeNameIn),
      isImplemented(false)
{
    try {
        if (nodeName) {
            CNodePtr pNode = (CNodePtr)pDrv->pNodeMap_->GetNode(nodeName);
            if (IsImplemented(pNode)) {
                isImplemented = true;
                pBase = pNode;
                if (propertyType == SPPropertyTypeUnknown) {
                    switch (pNode->GetPrincipalInterfaceType()) {
                        case intfIString:
                            propertyType = SPPropertyTypeString;
                             break;
                        case  intfIInteger:
                            propertyType = SPPropertyTypeInt;
                            break; 
                        case intfIFloat:
                            propertyType = SPPropertyTypeDouble;
                            break;
                        case intfIBoolean:
                            propertyType = SPPropertyTypeBoolean;
                            break;
                        case intfICommand:
                            propertyType = SPPropertyTypeCmd;
                            break;
                        case intfIEnumeration:
                            propertyType = SPPropertyTypeEnum;
                            break;
                        default:
                           break; 
                    }
                }
            }
        }
    }
    catch (Spinnaker::Exception &e) {
        printf("SPProperty::SPProperty exception %s\n", e.what());
    }
}

double ADSpinnaker::SPProperty::convertUnits(double inputValue, SPConvertDirection_t direction)
{
    double outputValue = inputValue;
    if ((nodeName == "ExposureTime") || 
        (nodeName == "TriggerDelay")) {
        // EPICS uses seconds, Spinnaker uses microseconds
        if (direction == SPConvertToEPICS)
            outputValue = inputValue / 1.e6;
        else
            outputValue = inputValue * 1.e6;
    } 
    else if (asynParam == pDrv->ADAcquirePeriod) {
        // EPICS uses period in seconds, Spinnaker uses rate in Hz
        outputValue = 1. / inputValue;
    }
    return outputValue;
}



int ADSpinnaker::SPProperty::convertUnits(int inputValue, SPConvertDirection_t direction)
{
    int outputValue = inputValue;
    if (nodeName == "AcquisitionMode") {
        // We want to use the EPICS enums
        if (direction == SPConvertToEPICS) {
            switch (inputValue) {
                case AcquisitionMode_SingleFrame: 
                    outputValue = ADImageSingle;
                    break;
                case AcquisitionMode_MultiFrame:
                    outputValue = ADImageMultiple;
                    break;
                case AcquisitionMode_Continuous:
                    outputValue = ADImageContinuous;
                    break;
            }
        } else {
            switch (inputValue) {
                case ADImageSingle:
                    outputValue = AcquisitionMode_SingleFrame;
                    break;
                case ADImageMultiple:
                    outputValue = AcquisitionMode_MultiFrame;
                    break;
                case ADImageContinuous:
                    outputValue = AcquisitionMode_Continuous;
                    break;
            }
        }
    }
    return outputValue;
}

asynStatus ADSpinnaker::SPProperty::getValue(void *pValue, bool setParam)
{
    static const char *functionName = "SPProperty::getValue";

    if (!isImplemented) return asynError;
    try {
        if (!IsAvailable(pBase)) {
             asynPrint(pDrv->pasynUserSelf, ASYN_TRACE_WARNING,
                "%s::%s Warning: node %s is not available\n",
                driverName, functionName, nodeName.c_str());
             return asynError;
        }
        if (!IsReadable(pBase)) {
             asynPrint(pDrv->pasynUserSelf, ASYN_TRACE_WARNING, 
                "%s::%s Warning: node %s is not readable\n",
                driverName, functionName, nodeName.c_str());
             return asynError;
        }
        switch (propertyType) {
            case SPPropertyTypeInt: {
                CIntegerPtr pNode = (CIntegerPtr)pBase;
                epicsInt32 value = (epicsInt32)pNode->GetValue();
                value = convertUnits(value, SPConvertToEPICS);
                if (pValue) *(epicsInt32*)pValue = value;
                if (setParam) pDrv->setIntegerParam(asynParam, value);
                break;
            }
            case SPPropertyTypeBoolean: {
                CBooleanPtr pNode = (CBooleanPtr)pBase;
                epicsInt32 value = (epicsInt32)pNode->GetValue();
                if (pValue) *(epicsInt32*)pValue = value;
                if (setParam) pDrv->setIntegerParam(asynParam, value);
                break;
            }
            case SPPropertyTypeDouble: {
                CFloatPtr pNode = (CFloatPtr)pBase;
                epicsFloat64 value = (epicsFloat64)pNode->GetValue();
                value = convertUnits(value, SPConvertToEPICS);
                if (pValue) *(epicsFloat64*)pValue = value;
                if (setParam) pDrv->setDoubleParam(asynParam, value);
                break;
            }
            case SPPropertyTypeEnum: {
                CEnumerationPtr pNode = (CEnumerationPtr)pBase;
                epicsInt32 value = (epicsInt32)pNode->GetIntValue();
                value = convertUnits(value, SPConvertToEPICS);
                if (pValue) *(epicsInt32*)pValue = value;
                if (setParam) pDrv->setIntegerParam(asynParam, value);
                break;
            }
            case SPPropertyTypeString: {
                CStringPtr pNode = (CStringPtr)pBase;
                std::string value = epicsStrDup((pNode->GetValue()).c_str());
                if (pValue) *(std::string *)pValue = value;
                if (setParam) pDrv->setStringParam(asynParam, value);
                break;
            }
            case SPPropertyTypeCmd: {
                break;
            }
            default:
                break;
        }
    }
    catch (Spinnaker::Exception &e) {
        asynPrint(pDrv->pasynUserSelf, ASYN_TRACE_ERROR, 
            "%s::%s node %s exception %s\n",
            driverName, functionName, nodeName.c_str(), e.what());
        return asynError;
    }
    return asynSuccess;
}


asynStatus ADSpinnaker::SPProperty::update()
{
    static const char *functionName = "SPProperty::update";

    if (!isImplemented) return asynError;
    try {
        if ((propertyType == SPPropertyTypeEnum) &&
            (!IsAvailable(pBase) || !IsWritable(pBase))) {
            char *enumStrings[1];
            int enumValues[1];
            int enumSeverities[1];
            enumStrings[0] = epicsStrDup("N.A.");
            enumValues[0] = 0;
            enumSeverities[0] = 0;
            pDrv->doCallbacksEnum(enumStrings, enumValues, enumSeverities, 
                                  1, asynParam, 0);
            return asynSuccess;
        }
        if (!IsAvailable(pBase)) {
             asynPrint(pDrv->pasynUserSelf, ASYN_TRACE_WARNING, 
                "%s::%s Warning: node %s is not available\n",
                driverName, functionName, nodeName.c_str());
             return asynError;
        }
        if (!IsReadable(pBase)) {
             asynPrint(pDrv->pasynUserSelf, ASYN_TRACE_WARNING, 
                "%s::%s Warning: node %s is not readable\n",
                driverName, functionName, nodeName.c_str());
             return asynError;
        }
        switch (propertyType) {
            case SPPropertyTypeDoubleMin: {
                CFloatPtr pNode = (CFloatPtr)pBase;
                epicsFloat64 value = pNode->GetMin();
                value = convertUnits(value, SPConvertToEPICS);
                pDrv->setDoubleParam(asynParam, value);
                break;
            }
            case SPPropertyTypeDoubleMax: {
                CFloatPtr pNode = (CFloatPtr)pBase;
                epicsFloat64 value = pNode->GetMax();
                value = convertUnits(value, SPConvertToEPICS);
                pDrv->setDoubleParam(asynParam, value);
                break;
            }
            case SPPropertyTypeEnum: {
                CEnumerationPtr pNode = (CEnumerationPtr)pBase;
                // We don't want to replace enum values for ADImageMode
                if (asynParam == pDrv->ADImageMode) return asynSuccess;
                NodeList_t entries;
                pNode->GetEntries(entries);
                int numEnums = (int)entries.size();
                char **enumStrings = new char*[numEnums];
                int *enumValues = new int[numEnums];
                int *enumSeverities = new int[numEnums];
                int j=0;
                for (int i=0; i<numEnums; i++) {
                    IEnumEntry *pEntry= dynamic_cast<IEnumEntry *>(entries[i]);
                    const char *pString = epicsStrDup((pEntry->GetSymbolic()).c_str());
                    if (IsAvailable(pEntry) && IsReadable(pEntry)) {
                        enumStrings[j] = epicsStrDup(pString);
                        long long entryValue = pEntry->GetValue();
                        enumValues[j] = (int)entryValue;
                        enumSeverities[j] = 0;
                        j++;
                    }
                }
                pDrv->doCallbacksEnum(enumStrings, enumValues, enumSeverities, 
                                      j, asynParam, 0);
                delete [] enumStrings; delete [] enumValues; delete [] enumSeverities;
                break;
            }
            default:
                break;
        }
    }
    catch (Spinnaker::Exception &e) {
        asynPrint(pDrv->pasynUserSelf, ASYN_TRACE_ERROR, 
            "%s::%s node %s exception %s\n",
            driverName, functionName, nodeName.c_str(), e.what());
        return asynError;
    }
    return asynSuccess;
}


asynStatus ADSpinnaker::SPProperty::setValue(void *pValue, void *pReadbackValue, bool setParam)
{
    static const char *functionName = "SPProperty::setValue";

    if (!isImplemented) return asynError;
    try {
        if (!IsAvailable(pBase)) {
             asynPrint(pDrv->pasynUserSelf, ASYN_TRACE_WARNING, 
                "%s::%s Warning: node %s is not available\n",
                driverName, functionName, nodeName.c_str());
             return asynError;
        }
        if (!IsWritable(pBase)) {
             asynPrint(pDrv->pasynUserSelf, ASYN_TRACE_WARNING, 
                "%s::%s Warning: node %s is not writable\n",
                driverName, functionName, nodeName.c_str());
             return asynError;
        }
        switch (propertyType) {
            case SPPropertyTypeInt: {
                CIntegerPtr pNode = (CIntegerPtr)pBase;
                epicsInt32 value;
                if (pValue)
                    value = *(epicsInt32*)pValue;
                else 
                    pDrv->getIntegerParam(asynParam, &value);
                value = convertUnits(value, SPConvertFromEPICS);
                // Check against the min and max
                int max = (int)pNode->GetMax();
                int min = (int)pNode->GetMin();
                int inc = (int)pNode->GetInc();
                if (inc != 1) {
                    value = (value/inc) * inc;
                }
                if (value < min) {
                   asynPrint(pDrv->pasynUserSelf, ASYN_TRACE_WARNING, 
                        "%s::%s Warning: node %s value %d is less than minimum %d, setting to minimum\n",
                        driverName, functionName, nodeName.c_str(), value, min);
                    value = min;
                }
                if (value > max) {
                   asynPrint(pDrv->pasynUserSelf, ASYN_TRACE_WARNING, 
                        "%s::%s Warning: node %s value %d is greater than maximum %d, setting to maximum\n",
                        driverName, functionName, nodeName.c_str(), value, max);
                    value = max;
                }
                pNode->SetValue(value);
                asynPrint(pDrv->pasynUserSelf, ASYN_TRACEIO_DRIVER, 
                    "%s::%s set property %s to %d\n",
                    driverName, functionName, nodeName.c_str(), value);
                if (IsReadable(pNode)) {
                    epicsInt32 readback = (epicsInt32)pNode->GetValue();
                    readback = convertUnits(readback, SPConvertToEPICS);
                    if (pReadbackValue) *(epicsInt32*)pReadbackValue = readback;
                    asynPrint(pDrv->pasynUserSelf, ASYN_TRACEIO_DRIVER, 
                        "%s::%s readback property %s is %d\n",
                        driverName, functionName, nodeName.c_str(), readback);
                    if (setParam) pDrv->setIntegerParam(asynParam, readback);
                }
                break;
            }
            case SPPropertyTypeBoolean: {
                CBooleanPtr pNode = (CBooleanPtr)pBase;
                epicsInt32 value;
                if (pValue) 
                    value = *(epicsInt32*)pValue;
                else
                    pDrv->getIntegerParam(asynParam, &value);
                *pNode = value ? true : false;
                asynPrint(pDrv->pasynUserSelf, ASYN_TRACEIO_DRIVER, 
                    "%s::%s set property %s to %d\n",
                    driverName, functionName, nodeName.c_str(), value);
                if (IsReadable(pNode)) {
                    epicsInt32 readback = (epicsInt32)pNode->GetValue();
                    if (pReadbackValue) *(epicsInt32*)pReadbackValue = readback;
                    asynPrint(pDrv->pasynUserSelf, ASYN_TRACEIO_DRIVER, 
                        "%s::%s readback property %s is %d\n",
                        driverName, functionName, nodeName.c_str(), readback);
                    if (setParam) pDrv->setIntegerParam(asynParam, readback);
                }
                break;
            }
            case SPPropertyTypeDouble: {
                CFloatPtr pNode = (CFloatPtr)pBase;
                epicsFloat64 value;
                if (pValue) 
                    value = *(epicsFloat64*)pValue;
                else
                    pDrv->getDoubleParam(asynParam, &value);
                value = convertUnits(value, SPConvertFromEPICS);
                // Check against the min and max
                double max = pNode->GetMax();
                double min = pNode->GetMin();
                if (value < min) {
                   asynPrint(pDrv->pasynUserSelf, ASYN_TRACE_WARNING, 
                        "%s::%s Warning: node %s value %f is less than minimum %f, setting to minimum\n",
                        driverName, functionName, nodeName.c_str(), value, min);
                    value = min;
                }
                if (value > max) {
                   asynPrint(pDrv->pasynUserSelf, ASYN_TRACE_WARNING, 
                        "%s::%s Warning: node %s value %f is greater than maximum %f, setting to maximum\n",
                        driverName, functionName, nodeName.c_str(), value, max);
                    value = max;
                }
                pNode->SetValue(value);
                asynPrint(pDrv->pasynUserSelf, ASYN_TRACEIO_DRIVER, 
                    "%s::%s set property %s to %f\n",
                    driverName, functionName, nodeName.c_str(), value);
                if (IsReadable(pNode)) {
                    double readback = pNode->GetValue();
                    readback = convertUnits(readback, SPConvertToEPICS);
                    if (pReadbackValue) *(epicsFloat64*)pReadbackValue = readback;
                    asynPrint(pDrv->pasynUserSelf, ASYN_TRACEIO_DRIVER, 
                        "%s::%s readback property %s is %f\n",
                        driverName, functionName, nodeName.c_str(), readback);
                    if (setParam) pDrv->setDoubleParam(asynParam, readback);
                }
                break;
            }
            case SPPropertyTypeEnum: {
                CEnumerationPtr pNode = (CEnumerationPtr)pBase;
                epicsInt32 value;
                if (pValue) 
                    value = *(epicsInt32*)pValue;
                else
                    pDrv->getIntegerParam(asynParam, &value);
                value = convertUnits(value, SPConvertFromEPICS);
                pNode->SetIntValue(value);
                asynPrint(pDrv->pasynUserSelf, ASYN_TRACEIO_DRIVER, 
                    "%s::%s set property %s to %d\n",
                    driverName, functionName, nodeName.c_str(), value);
                if (IsReadable(pNode)) {
                    epicsInt32 readback = (epicsInt32)pNode->GetIntValue();
                    readback = convertUnits(readback, SPConvertToEPICS);
                    if (pReadbackValue) *(epicsInt32*)pReadbackValue = readback;
                    asynPrint(pDrv->pasynUserSelf, ASYN_TRACEIO_DRIVER, 
                        "%s::%s readback property %s is %d\n",
                        driverName, functionName, nodeName.c_str(), readback);
                    if (setParam) pDrv->setIntegerParam(asynParam, readback);
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
                    pDrv->getStringParam(asynParam, temp);
                    value = temp.c_str();
                }
                pNode->SetValue(value);
                asynPrint(pDrv->pasynUserSelf, ASYN_TRACEIO_DRIVER, 
                    "%s::%s set property %s to %s\n",
                    driverName, functionName, nodeName.c_str(), value);
                if (IsReadable(pNode)) {
                    std::string readback = epicsStrDup((pNode->GetValue()).c_str());
                    if (pReadbackValue) *(std::string*)pReadbackValue = readback;
                    asynPrint(pDrv->pasynUserSelf, ASYN_TRACEIO_DRIVER, 
                        "%s::%s readback property %s is %s\n",
                        driverName, functionName, nodeName.c_str(), readback.c_str());
                    if (setParam) pDrv->setStringParam(asynParam, readback);
                }
                break;
            }
            case SPPropertyTypeCmd: {
                CCommandPtr pNode = (CCommandPtr)pBase;
                pNode->Execute();
                asynPrint(pDrv->pasynUserSelf, ASYN_TRACEIO_DRIVER, 
                    "%s::%s executed command %s\n",
                    driverName, functionName, nodeName.c_str());
                break;
            }
            default:
                break;
        }
    }
    catch (Spinnaker::Exception &e) {
        asynPrint(pDrv->pasynUserSelf, ASYN_TRACE_ERROR, 
            "%s::%s node %s exception %s\n",
            driverName, functionName, nodeName.c_str(), e.what());
        return asynError;
    }
    return asynSuccess;
}

asynStatus ADSpinnaker::createSPProperty(int *pAsynParam, asynParamType paramType, const char *asynDrvUser, const char *nodeName)
{
    createParam(asynDrvUser, paramType, pAsynParam);
    return createSPProperty(*pAsynParam, nodeName);
}

asynStatus ADSpinnaker::createSPProperty(int asynParam, const char *nodeName)
{
    SPProperty *pProp = new SPProperty(this, asynParam, nodeName);
    propertyList_[asynParam] = pProp;
    // If this is a double property then also create the min and max parameters and properties
    if (pProp->propertyType == SPPropertyTypeDouble) {
        const char *asynDrvUser;
        getParamName(asynParam, &asynDrvUser);
        std::string tempString(asynDrvUser);
        int temp;
        tempString += "_MIN";
        createParam(tempString.c_str(), asynParamFloat64, &temp);
        pProp = new SPProperty(this, temp, nodeName, SPPropertyTypeDoubleMin);
        propertyList_[temp] = pProp;
        tempString = asynDrvUser;
        tempString += "_MAX";
        createParam(tempString.c_str(), asynParamFloat64, &temp);
        pProp = new SPProperty(this, temp, nodeName, SPPropertyTypeDoubleMax);
        propertyList_[temp] = pProp;
    }
    return asynSuccess;    
}

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
    asynStatus status;
    
    if (traceMask == 0) traceMask = ASYN_TRACE_ERROR;
    pasynTrace->setTraceMask(pasynUserSelf, traceMask);


/*
    createParam(PGPacketSizeString,             asynParamInt32,   &PGPacketSize);
    createParam(PGPacketSizeActualString,       asynParamInt32,   &PGPacketSizeActual);
    createParam(PGMaxPacketSizeString,          asynParamInt32,   &PGMaxPacketSize);
    createParam(PGPacketDelayString,            asynParamInt32,   &PGPacketDelay);
    createParam(PGPacketDelayActualString,      asynParamInt32,   &PGPacketDelayActual);
    createParam(PGBandwidthString,              asynParamFloat64, &PGBandwidth);
    createParam(PGTimeStampModeString,          asynParamInt32,   &PGTimeStampMode);
*/
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

    // Construct property list.
    // First the properties that map into the base class parameters

    createSPProperty(ADImageMode,         "AcquisitionMode");
    createSPProperty(ADSerialNumber,      "DeviceSerialNumber");
    createSPProperty(ADFirmwareVersion,   "DeviceFirmwareVersion");
    createSPProperty(ADManufacturer,      "DeviceVendorName");
    createSPProperty(ADModel,             "DeviceModelName");
    createSPProperty(ADMaxSizeX,          "WidthMax");
    createSPProperty(ADMaxSizeY,          "HeightMax");
    createSPProperty(ADSizeX,             "Width");
    createSPProperty(ADSizeY,             "Height");
    createSPProperty(ADMinX,              "OffsetX");
    createSPProperty(ADMinY,              "OffsetY");
    createSPProperty(ADBinX,              "BinningHorizontal");
    createSPProperty(ADBinY,              "BinningVertical");
    createSPProperty(ADNumImages,         "AcquisitionFrameCount");
    createSPProperty(ADAcquireTime,       "ExposureTime");
    createSPProperty(ADAcquirePeriod,     "AcquisitionFrameRate");
    createSPProperty(ADGain,              "Gain");
    createSPProperty(ADTriggerMode,       "TriggerMode");
    createSPProperty(ADTemperatureActual, "DeviceTemperature");

    // Now the properties that are new to this driver.  createSProperty calls both createParam() and adds element to propertyList_.
    // These must be created in the same order as the property numbers    
    createSPProperty(&SPVideoMode,             asynParamInt32,   "SP_VIDEO_MODE",                "VideoMode");
    createSPProperty(&SPFrameRate,           asynParamFloat64,   "SP_FRAME_RATE",                "AcquisitionFrameRate"); 
    createSPProperty(&SPFrameRateAuto,         asynParamInt32,   "SP_FRAME_RATE_AUTO",           "AcquisitionFrameRateAuto");
    gcstring tempString = "AcquisitionFrameRateEnable";
    CNodePtr pBase = (CNodePtr)pNodeMap_->GetNode(tempString);
    if (!IsAvailable(pBase)) {
        tempString = "AcquisitionFrameRateEnabled";
        pBase = (CNodePtr)pNodeMap_->GetNode(tempString);
        if (!IsAvailable(pBase)) {
            printf("Error: neither AcquisitionFrameRateEnable nor AcquisitionFrameRateEnabled exist\n");
        }
    }
    createSPProperty(&SPFrameRateEnable,       asynParamInt32,   "SP_FRAME_RATE_ENABLE",         tempString);
    createSPProperty(&SPExposureAuto,          asynParamInt32,   "ACQ_TIME_AUTO",                "ExposureAuto");
    createSPProperty(&SPGainAuto,              asynParamInt32,   "GAIN_AUTO",                    "GainAuto");
    createSPProperty(&SPBlackLevel,            asynParamFloat64, "SP_BLACK_LEVEL",               "BlackLevel");
    createSPProperty(&SPBlackLevelAuto,        asynParamInt32,   "SP_BLACK_LEVEL_AUTO",          "BlackLevelAuto");
    createSPProperty(&SPBlackLevelBalanceAuto, asynParamInt32,   "SP_BLACK_LEVEL_BALANCE_AUTO",  "BlackLevelBalanceAuto");
    createSPProperty(&SPSaturation,            asynParamFloat64, "SP_SATURATION",                "Saturation");
    createSPProperty(&SPSaturationEnable,      asynParamInt32,   "SP_SATURATION_ENABLE",         "SaturationEnable");
    createSPProperty(&SPGamma,                 asynParamFloat64, "SP_GAMMA",                     "Gamma");
    createSPProperty(&SPGammaEnable,           asynParamInt32,   "SP_GAMMA_ENABLE",              "GammaEnable");
    createSPProperty(&SPSharpening,            asynParamFloat64, "SP_SHARPENING",                "Sharpening");
    createSPProperty(&SPSharpeningAuto,        asynParamInt32,   "SP_SHARPENING_AUTO",           "SharpeningAuto");
    createSPProperty(&SPSharpeningEnable,      asynParamInt32,   "SP_SHARPENING_ENABLE",         "SharpeningEnable");
    createSPProperty(&SPPixelFormat,           asynParamInt32,   "SP_PIXEL_FORMAT",              "PixelFormat");
    createParam("SP_CONVERT_PIXEL_FORMAT",     asynParamInt32,   &SPConvertPixelFormat);
    createSPProperty(&SPTriggerSource,         asynParamInt32,   "SP_TRIGGER_SOURCE",            "TriggerSource");
    createSPProperty(&SPTriggerActivation,     asynParamInt32,   "SP_TRIGGER_ACTIVATION",        "TriggerActivation");
    createSPProperty(&SPTriggerDelay,          asynParamFloat64, "SP_TRIGGER_DELAY",             "TriggerDelay");
    createSPProperty(&SPSoftwareTrigger,       asynParamInt32,   "SP_SOFTWARE_TRIGGER",          "TriggerSoftware");
    createSPProperty(&SPBalanceRatio,          asynParamFloat64, "SP_WHITE_BALANCE_RATIO",       "BalanceRatio");
    createSPProperty(&SPBalanceRatioSelector,  asynParamInt32,   "SP_WHITE_BALANCE_SELECTOR",    "BalanceRatioSelector");
    createSPProperty(&SPBalanceWhiteAuto,      asynParamInt32,   "SP_WHITE_BALANCE_AUTO",        "BalanceWhiteAuto");
    createSPProperty(&SPTransmitFailureCount,  asynParamInt32,   "SP_TRANSMIT_FAILURE_COUNT",    "TransmitFailureCount");
    // These are not properties but must be read from the TransportLayerStream class
    createParam("SP_BUFFER_UNDERRUN_COUNT",    asynParamInt32,   &SPBufferUnderrunCount);
    createParam("SP_FAILED_BUFFER_COUNT",      asynParamInt32,   &SPFailedBufferCount);
    createParam("SP_FAILED_PACKET_COUNT",      asynParamInt32,   &SPFailedPacketCount);

    updateSPProperties();

    report(stdout, 1);

    epicsInt32 iValue;
    
    /* Set initial values of some parameters */
    setIntegerParam(NDDataType, NDUInt8);
    setIntegerParam(NDColorMode, NDColorModeMono);
    setIntegerParam(NDArraySizeZ, 0);
    setIntegerParam(ADMinX, 0);
    setIntegerParam(ADMinY, 0);
    setStringParam(ADStringToServer, "<not used by driver>");
    setStringParam(ADStringFromServer, "<not used by driver>");
    setIntegerParam(SPTriggerSource, 0);

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
    try {
printf("shutdown, calling UnRegisterEvent()\n");
        pCamera_->UnregisterEvent(*pImageEventHandler_);
printf("shutdown, deleting pImageEventHandler\n");
        delete pImageEventHandler_;
printf("shutdown, setting pNodeMap=0\n");
        pNodeMap_ = 0;
        if (pImage_ != 0) {
printf("shutdown, deleting pImage\n");
            delete pImage_;
        }
printf("shutdown, calling pCamera_->DeInit()\n");
        pCamera_->DeInit();
printf("shutdown, calling camList_.Clear()\n");
        camList_.Clear();
printf("shutdown, calling system_->ReleaseInstance()\n");
        system_->ReleaseInstance();
    }
    catch (Spinnaker::Exception &e) {
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
    
        // Initialize camera
        pCamera_->Init();
        
        // Retrieve GenICam nodemap
        pNodeMap_ = &pCamera_->GetNodeMap();
    }

    catch (Spinnaker::Exception &e) {
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
    NDDataType_t dataType;
    NDColorMode_t colorMode;
    int convertPixelFormat;
    int numColors;
    size_t dims[3];
    ImageStatus imageStatus;
    PixelFormatEnums pixelFormat;
    int pixelSize;
    size_t dataSize, dataSizePG;
    void *pData;
    int nDims;
    static const char *functionName = "grabImage";

    try {
        while(1) {
            unlock();
            int recvSize = pCallbackMsgQ_->receive(&pImage_, sizeof(pImage_), 0.1);
            lock();
            if (recvSize == sizeof(pImage_)) {
                break;
            } else if (recvSize == -1) {
                // Timeout
                int acquire;
                getIntegerParam(ADAcquire, &acquire);
                if (acquire == 0) {
                    return asynError;
                } else {
                    continue;
                }
            } else {
                asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
                        "%s::%s error receiving from message queue\n",
                        driverName, functionName);
                return asynError;
            }
        }
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
     
//    timeStamp = pImage_->GetTimeStamp();    
//    pPGImage = pPGRawImage_;
    
        // Convert the pixel format if requested
        getIntegerParam(SPConvertPixelFormat, &convertPixelFormat);
        if (convertPixelFormat != SPPixelConvertNone) {
            PixelFormatEnums convertedFormat;
            switch (convertPixelFormat) {
                case SPPixelConvertMono8:
                    convertedFormat = PixelFormat_Mono8;
                    break;
                case SPPixelConvertMono16:
                    convertedFormat = PixelFormat_Mono16;
                    break;
                case SPPixelConvertRaw16:
                    convertedFormat = PixelFormat_Raw16;
                    break;
                case SPPixelConvertRGB8:
                    convertedFormat = PixelFormat_RGB8;
                    break;
                case SPPixelConvertRGB16:
                    convertedFormat = PixelFormat_RGB16;
                    break;
                default:
                    asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
                        "%s::%s Error: Unknown pixel conversion format %d\n",
                        driverName, functionName, convertPixelFormat);
                    convertedFormat = PixelFormat_Mono8;
                    break;
            }
    
            pixelFormat = pImage_->GetPixelFormat();
printf("Converting image from format 0x%x to format 0x%x\n", pixelFormat, convertedFormat);
            try {
                ImagePtr pConvertedImage = pImage_->Convert(convertedFormat);
                pImage_->Release();
                pImage_ = pConvertedImage;
            }
            catch (Spinnaker::Exception &e) {
                 asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, 
                     "%s::%s exception %s\n",
                 driverName, functionName, e.what());
            }
        }
    
        pixelFormat = pImage_->GetPixelFormat();
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
    catch (Spinnaker::Exception &e) {
      asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, 
          "%s::%s exception %s\n",
          driverName, functionName, e.what());
      return asynError;
    }
}

ADSpinnaker::SPProperty* ADSpinnaker::findProperty(int asynParam)
{
    static const char *functionName = "findProperty";
    std::map<int, SPProperty*>::iterator it;
    it = propertyList_.find(asynParam);
    if (it == propertyList_.end()) {
        asynPrint(pasynUserSelf, ASYN_TRACE_WARNING, 
            "%s::%s cannot find property with param=%d\n",
            driverName, functionName, asynParam);
        return 0;
    }
    return it->second;
}

asynStatus ADSpinnaker::getSPProperty(int asynParam, void *pValue, bool setParam)
{
    SPProperty* pElement = findProperty(asynParam);
    if (pElement == 0) {
        return asynError;
    }
    return pElement->getValue(pValue, setParam);
}

asynStatus ADSpinnaker::setSPProperty(int asynParam, void *pValue, void *pReadbackValue, bool setParam)
{
    SPProperty* pElement = findProperty(asynParam);
    if (pElement == 0) {
        return asynError;
    }
    return pElement->setValue(pValue, pReadbackValue, setParam);
}

asynStatus ADSpinnaker::updateSPProperties()
{
    //static const char *functionName = "updateSPProperties";
    SPProperty *pProperty;
epicsTimeStamp tStart, tEnd;
epicsTimeGetCurrent(&tStart);
    std::map<int, SPProperty*>::iterator it;
    for (it=propertyList_.begin(); it != propertyList_.end(); it++) {
        pProperty = it->second;
        pProperty->getValue();
        pProperty->update();
    }
epicsTimeGetCurrent(&tEnd);
printf("updateSPProperties, time=%f\n", epicsTimeDiffInSeconds(&tEnd, &tStart));
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
    static const char *functionName = "writeInt32";

    // Set the value in the parameter library.  This may change later but that's OK
    status = setIntegerParam(function, value);

    if (function == ADAcquire) {
        if (value) {
            // start acquisition
            status = startCapture();
        } else {
            status = stopCapture();
        }

    } 
    else if ((function == ADSizeX)       ||
             (function == ADSizeY)       ||
             (function == ADMinX)        ||
             (function == ADMinY)        ||
             (function == ADBinX)        ||
             (function == ADBinY)        ||
             (function == ADImageMode)   ||
             (function == ADNumImages)   ||
             (function == NDDataType)) {    
        status = setImageParams();
    } 
    else if (function == ADReadStatus) {
        status = readStatus();
    } 
    else if (function < FIRST_SP_PARAM) {
        // If this parameter belongs to a base class call its method
        status = ADDriver::writeInt32(pasynUser, value);
    } 
    else {
        setSPProperty(function, &value);
        updateSPProperties();
    }

    asynPrint(pasynUserSelf, ASYN_TRACEIO_DRIVER, 
        "%s::%s function=%d, value=%d, status=%d\n",
        driverName, functionName, function, value, status);
            
    callParamCallbacks();
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
    static const char *functionName = "writeFloat64";
    
    // Set the value in the parameter library.  This may change later but that's OK
    status = setDoubleParam(function, value);

    setSPProperty(function);
    updateSPProperties();

    if (function < FIRST_SP_PARAM) {
        // If this parameter belongs to a base class call its method
        status = ADDriver::writeFloat64(pasynUser, value);
    } 
    
    asynPrint(pasynUser, ASYN_TRACEIO_DRIVER, 
        "%s::%s function=%d, value=%f, status=%d\n",
        driverName, functionName, function, value, status);
    callParamCallbacks();
    return status;
}

asynStatus ADSpinnaker::readEnum(asynUser *pasynUser, char *strings[], int values[], int severities[], 
                               size_t nElements, size_t *nIn)
{
    int function = pasynUser->reason;
    int numEnums;
    int i;
    long long entryValue;
    static const char *functionName = "readEnum";

    SPProperty *pElement = findProperty(function);

    if (pElement == 0) {
        return asynError;
    }
    if ((pElement->propertyType != SPPropertyTypeEnum) && 
        (pElement->propertyType != SPPropertyTypeUnknown)) {
        return asynError;
    }
    // There are a few enums we don't want to autogenerate the values
    if ((function == SPConvertPixelFormat) ||
        (function == ADImageMode)) {
        return asynError;
    }
    gcstring nodeName = pElement->nodeName;

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
                if (strings[*nIn]) free(strings[*nIn]);
                strings[*nIn] = epicsStrDup(pString);
                entryValue = pEntry->GetValue();
                values[*nIn] = (int)entryValue;
                severities[*nIn] = 0;
                (*nIn)++;
            } else {
            }
        }
    }

    catch (Spinnaker::Exception &e) {
      asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, 
          "%s::%s node %s exception %s\n",
          driverName, functionName, nodeName.c_str(), e.what());
      return asynError;
    }
    return asynSuccess;   
}


asynStatus ADSpinnaker::setImageParams()
{
    //static const char *functionName = "setImageParams";
    
    //bool resumeAcquire;

    if (!pCamera_) return asynError;

    setSPProperty(SPVideoMode);
    setSPProperty(ADImageMode);
    setSPProperty(ADNumImages);
    setSPProperty(ADSizeX);
    setSPProperty(ADSizeY);
    setSPProperty(ADMinX);
    setSPProperty(ADMinY);
    setSPProperty(ADBinX);
    setSPProperty(ADBinY);

    // We read these back after setting all of them in case one setting affects another
    getSPProperty(SPVideoMode);
    getSPProperty(ADImageMode);
    getSPProperty(ADNumImages);
    getSPProperty(ADSizeX);
    getSPProperty(ADSizeY);
    getSPProperty(ADMinX);
    getSPProperty(ADMinY);
    getSPProperty(ADBinX);
    getSPProperty(ADBinY);

    return asynSuccess;
}


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
    catch (Spinnaker::Exception &e) {
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
        // Need to empty the message queue it could have some images in it
        while(pCallbackMsgQ_->tryReceive(&pImage_, sizeof(pImage_)) != -1) {
        }
    }
    catch (Spinnaker::Exception &e) {
      asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, 
          "%s::%s exception %s\n",
          driverName, functionName, e.what());
      return asynError;
    }
    return asynSuccess;
}


asynStatus ADSpinnaker::readStatus()
{
    static const char *functionName = "readStatus";

    try {
        const TransportLayerStream& camInfo = pCamera_->TLStream;
        getSPProperty(ADTemperatureActual);
        setIntegerParam(SPBufferUnderrunCount, (int)camInfo.StreamBufferUnderrunCount.GetValue());
        setIntegerParam(SPFailedBufferCount,   (int)camInfo.StreamFailedBufferCount.GetValue());
        if (camInfo.StreamType.GetIntValue() == StreamType_GEV) {
            setIntegerParam(SPFailedPacketCount,   (int)camInfo.GevFailedPacketCount.GetValue());
        }
    }
    catch (Spinnaker::Exception &e) {
        asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, 
            "%s::%s exception %s\n",
            driverName, functionName, e.what());
        return asynError;
    }
    callParamCallbacks();
    return asynSuccess;
}

// This helper function deals with output indentation, of which there is a lot.
void indent(FILE *fp, unsigned int level)
{
  for (unsigned int i=0; i<level; i++) {
    fprintf(fp, "   ");
  }
}

gcstring ADSpinnaker::getValueAsString(INodeMap *pNodeMap, gcstring nodeName, gcstring & displayName)
{
    static const char *functionName = "getValueAsString";
    gcstring valueString = "Not available";
    displayName = "Unknown";
    try {
        CNodePtr pBase = (CNodePtr)pNodeMap->GetNode(nodeName);
        if (IsAvailable(pBase) && IsReadable(pBase)) {
            displayName = pBase->GetDisplayName();
            switch (pBase->GetPrincipalInterfaceType()) {
                case intfIString: {
                    CStringPtr pNode = static_cast<CStringPtr>(pBase);
                    valueString = pNode->GetValue();
                     break;
                    }
                case  intfIInteger: {
                    CIntegerPtr pNode = static_cast<CIntegerPtr>(pBase);
                    valueString = pNode->ToString();
                    break; 
                    }
        
                case intfIFloat: {
                    CFloatPtr pNode = static_cast<CFloatPtr>(pBase);
                    valueString = pNode->ToString();
                    break;
                    }
                case intfIBoolean: {
                    CBooleanPtr pNode = static_cast<CBooleanPtr>(pBase);
                    valueString = pNode->ToString();
                    break;
                    }
                case intfICommand: {
                    CCommandPtr pNode = static_cast<CCommandPtr>(pBase);
                    valueString = pNode->GetToolTip();
                    break;
                    }
                case intfIEnumeration: {
                    CEnumerationPtr pNode = static_cast<CEnumerationPtr>(pBase);
                    CEnumEntryPtr pEntry = pNode->GetCurrentEntry();
                    valueString = pEntry->GetSymbolic();
                    break;
                   }
                default:
                   break; 
            }
        }
    } 
    catch (Spinnaker::Exception &e) {
      asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, 
          "%s::%s node %s exception %s\n",
          driverName, functionName, nodeName.c_str(), e.what());
      valueString = "Value not available";
    }
    return valueString;
}

void ADSpinnaker::reportNode(FILE *fp, INodeMap *pNodeMap, gcstring nodeName, int level)
{
    gcstring displayName;
    gcstring value = getValueAsString(pNodeMap, nodeName, displayName);
    
     indent(fp, level);
     fprintf(fp, "%s (%s):%s\n", displayName.c_str(), nodeName.c_str(), value.c_str());
}


/** Print out a report; calls ADDriver::report to get base class report as well.
  * \param[in] fp File pointer to write output to
  * \param[in] details Level of detail desired.  If >1 prints information about 
               supported video formats and modes, etc.
 */

void ADSpinnaker::report(FILE *fp, int details)
{
    int numCameras;
    int i;
    static const char *functionName = "report";

    try {    
        numCameras = camList_.GetSize();
        fprintf(fp, "\nNumber of cameras detected: %d\n", numCameras);
        if (details <1) return;
        for (i=0; i<numCameras; i++) {
            CameraPtr pCamera;
            pCamera = camList_.GetByIndex(i);
            INodeMap *pNodeMap = &pCamera->GetTLDeviceNodeMap();
    
            fprintf(fp, "Camera %d\n", i);
            reportNode(fp, pNodeMap, "DeviceVendorName", 1);
            reportNode(fp, pNodeMap, "DeviceModelName", 1);
            reportNode(fp, pNodeMap, "DeviceSerialNumber", 1);
            reportNode(fp, pNodeMap, "DeviceVersion", 1);
            reportNode(fp, pNodeMap, "DeviceType", 1);
        }
        // Print out propertyList_
        SPProperty *pProperty;
        std::map<int, SPProperty*>::iterator it;
        fprintf(fp, "Property list\n");
        for (it=propertyList_.begin(); it != propertyList_.end(); it++) {
            pProperty = it->second;
            fprintf(fp, "\n      Node name: %s\n", pProperty->nodeName.c_str());
            fprintf(fp, "      asynParam: %d\n", pProperty->asynParam);
            fprintf(fp, "  isImplemented: %d\n", pProperty->isImplemented);
            gcstring displayName;
            gcstring value = getValueAsString(pNodeMap_, pProperty->nodeName, displayName);
            fprintf(fp, "          value: %s\n", value.c_str());
        }
    }
    catch (Spinnaker::Exception &e) {
      asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, 
          "%s::%s exception %s\n",
          driverName, functionName, e.what());
    }
    
    ADDriver::report(fp, details);
    return;
}

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

