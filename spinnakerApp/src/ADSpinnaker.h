#ifndef ADSPINNAKER_H
#define ADSPINNAKER_H

#include <epicsEvent.h>

#include <ADGenICam.h>
#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;

class ImageEventHandler : public ImageEvent
{
public:

    ImageEventHandler(epicsMessageQueue *pMsgQ) 
     : pMsgQ_(pMsgQ)
    {}
    ~ImageEventHandler() {}
  
    void OnImageEvent(ImagePtr image) {
        ImagePtr *imagePtrAddr = new ImagePtr(image);
  
        if (pMsgQ_->send(&imagePtrAddr, sizeof(imagePtrAddr)) != 0) {
            printf("OnImageEvent error calling pMsgQ_->send()\n");
        }
    }
  
private:
    epicsMessageQueue *pMsgQ_;

};


/** Main driver class inherited from areaDetectors ADDriver class.
 * One instance of this class will control one camera.
 */
class ADSpinnaker : public ADGenICam
{
public:
    ADSpinnaker(const char *portName, int cameraId, int traceMask, int memoryChannel,
                size_t maxMemory, int priority, int stackSize);

    // virtual methods to override from ADGenICam
    //virtual asynStatus writeInt32( asynUser *pasynUser, epicsInt32 value);
    //virtual asynStatus writeFloat64( asynUser *pasynUser, epicsFloat64 value);
    virtual asynStatus readEnum(asynUser *pasynUser, char *strings[], int values[], int severities[], 
                                size_t nElements, size_t *nIn);
    void report(FILE *fp, int details);
    virtual GenICamFeature *createFeature(GenICamFeatureSet *set, 
                                          std::string const & asynName, asynParamType asynType, int asynIndex,
                                          std::string const & featureName, GCFeatureType_t featureType);
    INodeMap *getNodeMap();
    
    /**< These should be private but are called from C callback functions, must be public. */
    void imageGrabTask();
    void shutdown();

private:
    int SPVideoMode;
#define FIRST_SP_PARAM SPVideoMode
    int SPConvertPixelFormat;
    int SPTransmitFailureCount;
    int SPBufferUnderrunCount;
    int SPFailedBufferCount;
    int SPFailedPacketCount;
    int SPTimeStampMode;
    int SPUniqueIdMode;
    int SPColorProcessEnabled;

//    int PGPacketSize;             /** Size of data packets from camera                (int32 write/read) */
//    int PGPacketSizeActual;       /** Size of data packets from camera                (int32 write/read) */
//    int PGMaxPacketSize;          /** Maximum size of data packets from camera        (int32 write/read) */
//    int PGPacketDelay;            /** Packet delay in usec from camera, GigE only     (int32 write/read) */
//    int PGPacketDelayActual;      /** Packet delay in usec from camera, GigE only     (int32 read) */
//    int PGBandwidth;              /** Bandwidth in MB/s                               (float64 read) */

    /* Local methods to this class */
    asynStatus grabImage();
    asynStatus startCapture();
    asynStatus stopCapture();
    asynStatus connectCamera();
    asynStatus disconnectCamera();
    asynStatus readStatus();
    void imageEventCallback(ImagePtr pImage);
    void reportNode(FILE *fp, INodeMap *pNodeMap, gcstring nodeName, int level);

    /* Data */
    int cameraId_;
    int memoryChannel_;
    
    INodeMap *pNodeMap_;    
    SystemPtr system_;
    CameraList camList_;
    CameraPtr pCamera_;
    ImageEventHandler *pImageEventHandler_;

    bool stopAcquisition_;
    int exiting_;
    epicsEventId startEventId_;
    epicsMessageQueue *pCallbackMsgQ_;
    NDArray *pRaw_;
    int uniqueId_;
};

#endif

