#ifndef ADSPINNAKER_H
#define ADSPINNAKER_H

#include <epicsEvent.h>

#include <ADGenICam.h>
#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;

#define SPConvertPixelFormatString  "SP_CONVERT_PIXEL_FORMAT"   // asynParamInt32, R/W
#define SPLostFrameCountString      "SP_LOST_FRAME_COUNT"       // asynParamInt32, R/O
#define SPFailedBufferCountString   "SP_FAILED_BUFFER_COUNT"    // asynParamInt32, R/O
#define SPFailedPacketCountString   "SP_FAILED_PACKET_COUNT"    // asynParamInt32, R/O
#define SPTimeStampModeString       "SP_TIME_STAMP_MODE"        // asynParamInt32, R/O
#define SPUniqueIdModeString        "SP_UNIQUE_ID_MODE"         // asynParamInt32, R/O

class ADSpinnakerImageEventHandler : public ImageEventHandler
{
public:

    ADSpinnakerImageEventHandler(epicsMessageQueue *pMsgQ) 
     : pMsgQ_(pMsgQ)
    {}
    ~ADSpinnakerImageEventHandler() {}
  
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
    ADSpinnaker(const char *portName, int cameraId, int traceMask,
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
    int SPConvertPixelFormat;
#define FIRST_SP_PARAM SPConvertPixelFormat
    int SPLostFrameCount;
    int SPFailedBufferCount;
    int SPFailedPacketCount;
    int SPTimeStampMode;
    int SPUniqueIdMode;
    int SPFrameRateEnable;

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
    
    INodeMap *pNodeMap_;    
    SystemPtr system_;
    CameraList camList_;
    CameraPtr pCamera_;
    ImageEventHandler *pImageEventHandler_;

    int exiting_;
    epicsEventId startEventId_;
    epicsMessageQueue *pCallbackMsgQ_;
    NDArray *pRaw_;
    int uniqueId_;
};

#endif

