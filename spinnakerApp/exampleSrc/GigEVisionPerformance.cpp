//=============================================================================
// Copyright (c) 2001-2019 FLIR Systems, Inc. All Rights Reserved.
//
// This software is the confidential and proprietary information of FLIR
// Integrated Imaging Solutions, Inc. ("Confidential Information"). You
// shall not disclose such Confidential Information and shall use it only in
// accordance with the terms of the license agreement you entered into
// with FLIR Integrated Imaging Solutions, Inc. (FLIR).
//
// FLIR MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
// SOFTWARE, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. FLIR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================

/**
 *	@example GigEVisionPerformance.cpp
 *
 *	@brief GigEVisionPerformance.cpp measures GigE Vision performance. It is built
 *	on top of Acquisition example.
 *
 *	This example measures CPU related performance statistics and print them out at the
 *	end.
 *
 */

#include "Spinnaker.h"
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include "CpuUtil.h"
#include "GigEVisionPerformance.h"

using namespace std;
using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;

CpuUtil::CpuUsageInfo cpuUsageInfo;

// Following parameters can be configured through command-line arguments.
// Use "-?" argument to see detailed usage information.
int TestDuration = 0; // seconds
char* PixelFormatToSet = nullptr;
int PacketSizeToSet = 9000;
int PacketDelayToSet = 0;
bool IsRelease = false; // whether to call release() on the image Pointer or let it be done implicitly by the grab loop
bool UseDuration = false;
bool UseMaxFramerate = false;
float UserSetFramerate = 0.0;
int NumImagesToGrab = 100;

// Supported command-line arguments
const char* argNumImages = "-numimages";
const char* argDuration = "-duration";
const char* argRelease = "-callrelease";
const char* argBayerRG = "-bayerrg";
const char* argPacketSize = "-packetsize";
const char* argPacketDelay = "-packetdelay";
const char* argMaxFrames = "-maxfps";
const char* argUserSetFrames = "-fps";
const char* argPrintUsage = "-?";

void PrintUsage()
{
    cout << argPrintUsage << "			<Displays this usage information>" << endl;
    cout << argNumImages << "		<Optional. Sets number of images to stream>" << endl;
    cout << argDuration << "		<Optional. Sets time in seconds to stream>" << endl;
    cout << argRelease << "		<Optional. Calls Release() explicitly on grabbed image pointer if set>" << endl;
    cout << argBayerRG
         << "		<Optional. Sets Pixel Format to BayerRG 8 or BayerRG16 for color cameras, using 8 or 16 as "
            "argument>"
         << endl;
    cout << argPacketSize << "		<Optional. Sets desired Packet Size>" << endl;
    cout << argPacketDelay << "		<Optional. Sets desired Packet Delay>" << endl;
    cout << argMaxFrames << "			<Optional. Sets AcquisitionFramerate to max>" << endl;
    cout << argUserSetFrames << "			<Optional. Sets desired AcquisitionFramerate>" << endl;
    cout << endl;
}

bool ParseArguments(int argc, char* argv[])
{
    cout << endl << "*** PARSING ARGUMENTS ***" << endl << endl;

    cout << "Use '-?' to see list of supported arguments." << endl << endl;

    if (argc == 1)
    {
        // Grabbing 100 images using maximum framerate by default.
        cout << "Grabbing 100 images using maximum framerate..." << endl << endl;
        NumImagesToGrab = 100;
        UseMaxFramerate = true;

        // Continue with default parameters
        return true;
    }

    for (int argument = 1; argument < argc; ++argument)
    {
        if (strncmp(argv[argument], argPrintUsage, strlen(argPrintUsage)) == 0)
        {
            // Print usage information
            PrintUsage();
            return false;
        }

        if (strncmp(argv[argument], argDuration, strlen(argDuration)) == 0)
        {
            if (argument + 1 <= argc)
            {
                UseDuration = true;
                TestDuration = atoi(argv[argument + 1]);
                argument++;
            }
        }

        if (strncmp(argv[argument], argNumImages, strlen(argNumImages)) == 0)
        {
            if (argument + 1 <= argc)
            {
                UseDuration = false;
                NumImagesToGrab = atoi(argv[argument + 1]);
                argument++;
            }
        }

        if (strncmp(argv[argument], argPacketSize, strlen(argPacketSize)) == 0)
        {
            if (argument + 1 <= argc)
            {
                PacketSizeToSet = atoi(argv[argument + 1]);
                argument++;
            }
        }
        if (strncmp(argv[argument], argPacketDelay, strlen(argPacketDelay)) == 0)
        {
            if (argument + 1 <= argc)
            {
                PacketDelayToSet = atoi(argv[argument + 1]);
                argument++;
            }
        }
        if (strncmp(argv[argument], argUserSetFrames, strlen(argUserSetFrames)) == 0)
        {
            if (argument + 1 <= argc)
            {
                UserSetFramerate = stof(argv[argument + 1]);
                argument++;
            }
        }
        if (strncmp(argv[argument], argBayerRG, strlen(argBayerRG)) == 0)
        {
            if (argument + 1 <= argc)
            {
                int bayerRG_bits = atoi(argv[argument + 1]);
                switch (bayerRG_bits)
                {
                case 8:
                    PixelFormatToSet = "BayerRG8";
                    break;
                case 16:
                    PixelFormatToSet = "BayerRG16";
                    break;
                default:
                    cout << "User did not specify BayerRG 8 or BayerRG 16" << endl << endl;
                }
                cout << "Using Pixel Format: " << PixelFormatToSet << endl << endl;
                argument++;
            }
        }
        if (strncmp(argv[argument], argRelease, strlen(argRelease)) == 0)
        {
            IsRelease = true;
        }
        if (strncmp(argv[argument], argMaxFrames, strlen(argMaxFrames)) == 0)
        {
            UseMaxFramerate = true;
        }
    }

    return true;
}

void getCameraCategory(INodeMap& nodeMap, string categoryString)
{
    try
    {
        stringstream cameraFeaturesFromCategoryStream;

        cout << endl << "*** Get Camera Config.. " << categoryString << endl;

        FeatureList_t features;
        CCategoryPtr category = nodeMap.GetNode(categoryString.c_str());
        category->GetFeatures(features);

        FeatureList_t::const_iterator it;
        for (it = features.begin(); it != features.end(); ++it)
        {
            CNodePtr pfeatureNode = *it;
            CValuePtr pValue = (CValuePtr)pfeatureNode;

            if (IsReadable(pValue))
            {
                gcstring featureName = pfeatureNode->GetName();
                gcstring sensorString = pValue->ToString();

                cameraFeaturesFromCategoryStream << pfeatureNode->GetName() << " : ";
                cameraFeaturesFromCategoryStream << sensorString;
                cameraFeaturesFromCategoryStream << endl;
            }
        }

        std::string stringToReturn = cameraFeaturesFromCategoryStream.str();

        cout << stringToReturn << endl;
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Exception in getCameraCategory():" << e.what() << endl;
    }
}

void PrintDataStreamInfo(const Spinnaker::CameraPtr pCamera)
{
    try
    {
        cout << endl << endl << "*** DATASTREAM STATS ***" << endl << endl;

        stringstream outSS;

        // Get model name
        const TransportLayerStream& camInfo = pCamera->TLStream;

        if (camInfo.StreamID != NULL)
            outSS << "Stream ID: " << camInfo.StreamID.ToString() << endl;
        if (camInfo.StreamType != NULL)
            outSS << "Stream Type: " << camInfo.StreamType.ToString() << endl;
        if (camInfo.StreamBufferCountResult != NULL)
            outSS << "Stream Buffer Count: " << camInfo.StreamBufferCountResult.ToString() << endl;
        if (camInfo.StreamBufferHandlingMode != NULL)
            outSS << "Stream Buffer Handling Mode: " << camInfo.StreamBufferHandlingMode.ToString() << endl;

        if (camInfo.GevTotalPacketCount != NULL)
            outSS << "Stream Packets Received: " << camInfo.GevTotalPacketCount.ToString() << endl;
        if (camInfo.GevFailedPacketCount != NULL)
            outSS << "Stream Packets Missed: " << camInfo.GevFailedPacketCount.ToString() << endl;
        if (camInfo.GevResendPacketCount != NULL)
            outSS << "Stream Retransmitted Packets: " << camInfo.GevResendPacketCount.ToString() << endl;
        if (camInfo.StreamFailedBufferCount != NULL)
            outSS << "Stream Failed Buffer Count: " << camInfo.StreamFailedBufferCount.ToString() << endl;
        if (camInfo.StreamLostFrameCount != NULL)
            outSS << "Stream Buffer Underrun Count: " << camInfo.StreamLostFrameCount.ToString() << endl;
        if (camInfo.GevResendRequestCount != NULL)
            outSS << "Stream Retransmitted Packets Requested: " << camInfo.GevResendRequestCount.ToString() << endl;

        cout << outSS.str() << endl;
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Problem reading device info : " << e.GetError() << " - " << e.GetErrorMessage() << endl;
    }
}

// This function acquires and saves x images from a device.
int AcquireImages(CameraPtr pCam, INodeMap& nodeMap, INodeMap& nodeMapGenTL, int numImagesToAcquire, int iteration)
{
    int result = 0;
    int fail_image = 0;

    cout << endl << endl << "*** ACQUIRING " << numImagesToAcquire << " IMAGES ***" << endl << endl;

    try
    {
        // Retrieve enumeration node from nodemap
        CEnumerationPtr ptrAcquisitionMode = nodeMap.GetNode("AcquisitionMode");
        if (!IsAvailable(ptrAcquisitionMode) || !IsWritable(ptrAcquisitionMode))
        {
            cout << "Unable to set acquisition mode to continuous (enum retrieval). Aborting..." << endl << endl;
            return -1;
        }

        // Retrieve entry node from enumeration node
        CEnumEntryPtr ptrAcquisitionModeContinuous = ptrAcquisitionMode->GetEntryByName("Continuous");
        if (!IsAvailable(ptrAcquisitionModeContinuous) || !IsReadable(ptrAcquisitionModeContinuous))
        {
            cout << "Unable to set acquisition mode to continuous (entry retrieval). Aborting..." << endl << endl;
            return -1;
        }

        // Retrieve integer value from entry node
        int64_t acquisitionModeContinuous = ptrAcquisitionModeContinuous->GetValue();

        // Set integer value from entry node as new value of enumeration node
        ptrAcquisitionMode->SetIntValue(acquisitionModeContinuous);

        cout << "Acquisition mode set to Continuous..." << endl;

        // Begin acquiring images
        pCam->BeginAcquisition();

        cout << "Acquiring images..." << endl;

        const unsigned int k_numImages = numImagesToAcquire;

        ImagePtr pResultImage;

        // Start Capturing CPU Stats
        CpuUtil::StartCpuTracing(&cpuUsageInfo);
        for (unsigned int imageCnt = 0; imageCnt < k_numImages; imageCnt++)
        {
            try
            {
                pResultImage = pCam->GetNextImage(1000);

                if (IsRelease)
                {
                    pResultImage->Release();
                }
            }
            catch (Spinnaker::Exception& e)
            {
                cout << "Error: " << e.what() << endl;
                result = -1;
            }
        }

        // Stop Capturing CPU Stats
        CpuUtil::StopCpuTracing(&cpuUsageInfo);

        // Fetch CPU Stats
        string cpuUsageString = CpuUtil::GetCpuStats(&cpuUsageInfo);

        // End Acquisition
        pCam->EndAcquisition();

        cout << "Finished acquiring images..." << endl;
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Error: " << e.what() << endl;
        result = -1;
    }

    return result;
}

// This function prints the device information of the camera from the transport
// layer; please see NodeMapInfo example for more in-depth comments on printing
// device information from the nodemap.
int PrintDeviceInfo(INodeMap& nodeMap)
{
    int result = 0;

    cout << endl << "*** DEVICE INFORMATION ***" << endl << endl;

    try
    {
        FeatureList_t features;
        CCategoryPtr category = nodeMap.GetNode("DeviceInformation");
        if (IsAvailable(category) && IsReadable(category))
        {
            category->GetFeatures(features);
            FeatureList_t::const_iterator it;
            for (it = features.begin(); it != features.end(); ++it)
            {
                CNodePtr pfeatureNode = *it;
                cout << pfeatureNode->GetName() << " : ";
                CValuePtr pValue = (CValuePtr)pfeatureNode;
                cout << (IsReadable(pValue) ? pValue->ToString() : "Node not readable");
                cout << endl;
            }
        }
        else
        {
            cout << "Device control information not available." << endl;
        }
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Error: " << e.what() << endl;
        result = -1;
    }

    return result;
}

void PrintCPUUsage()
{
    cout << endl << "*** CPU USAGE STATS ***" << endl << endl;

    stringstream outSS;
    outSS << "Kernel Time: " << cpuUsageInfo.kernelSystemTime.wHour << "H"
          << ":" << cpuUsageInfo.kernelSystemTime.wMinute << "M"
          << ":" << cpuUsageInfo.kernelSystemTime.wSecond << "S"
          << ":" << cpuUsageInfo.kernelSystemTime.wMilliseconds << "ms"
          << "\n";
    outSS << "User Time: " << cpuUsageInfo.userSystemTime.wHour << "H"
          << ":" << cpuUsageInfo.userSystemTime.wMinute << "M"
          << ":" << cpuUsageInfo.userSystemTime.wSecond << "S"
          << ":" << cpuUsageInfo.userSystemTime.wMilliseconds << "ms"
          << "\n";

    outSS << "Kernel Time: "
          << ((cpuUsageInfo.kernelSystemTime.wHour * 60 * 60 * 1000) +
              (cpuUsageInfo.kernelSystemTime.wMinute * 60 * 1000) + (cpuUsageInfo.kernelSystemTime.wSecond * 1000) +
              (cpuUsageInfo.kernelSystemTime.wMilliseconds))
          << "ms"
          << "\n";
    outSS << "User Time: "
          << ((cpuUsageInfo.userSystemTime.wHour * 60 * 60 * 1000) + (cpuUsageInfo.userSystemTime.wMinute * 60 * 1000) +
              (cpuUsageInfo.userSystemTime.wSecond * 1000) + (cpuUsageInfo.userSystemTime.wMilliseconds))
          << "ms"
          << "\n";

    outSS << "CPU Usage: " << cpuUsageInfo.cpuPercentage << "%\n";
    outSS << "Total Time: " << cpuUsageInfo.elapsedTime << " seconds"
          << "\n";
    cout << outSS.str() << endl;
}

void PrintAllNodes(CameraPtr pCam)
{
    INodeMap& nodeMap = pCam->GetNodeMap();
    INodeMap& nodeMapDevice = pCam->GetTLDeviceNodeMap();
    INodeMap& nodeMapStream = pCam->GetTLStreamNodeMap();
    PrintDeviceInfo(nodeMap);
    PrintDeviceInfo(nodeMapDevice);
    PrintDeviceInfo(nodeMapStream);
}

bool EnableManualFramerate(CameraPtr pCam)
{
    INodeMap& NodeMap = pCam->GetNodeMap();

    // Turning AcquisitionFrameRateEnable on
    CBooleanPtr ptrFrameRateEnable = NodeMap.GetNode("AcquisitionFrameRateEnable");
    if (ptrFrameRateEnable == nullptr)
    {
        // AcquisitionFrameRateEnabled is used for Gen2 devices
        ptrFrameRateEnable = NodeMap.GetNode("AcquisitionFrameRateEnabled");
    }

    if (IsAvailable(ptrFrameRateEnable) && IsWritable(ptrFrameRateEnable))
    {
        ptrFrameRateEnable->SetValue(true);

        cout << "AcquisitionFrameRateEnable set to True" << endl;
    }

    // Turning AcquisitionFrameRateAuto off
    CEnumerationPtr ptrFrameRateAuto = NodeMap.GetNode("AcquisitionFrameRateAuto");
    if (!IsAvailable(ptrFrameRateAuto) || !IsWritable(ptrFrameRateAuto))
    {
        cout << "Unable to set AcquisitionFrameRateAuto..." << endl << endl;
        return false;
    }

    CEnumEntryPtr ptrFrameRateAutoModeOff = ptrFrameRateAuto->GetEntryByName("Off");
    if (!IsAvailable(ptrFrameRateAutoModeOff) || !IsReadable(ptrFrameRateAutoModeOff))
    {
        cout << "Unable to set AcquisitionFrameRateAuto to OFF. Aborting..." << endl << endl;
        return false;
    }

    // Retrieve integer value from entry node
    const int64_t valueFrameRateAutoOff = ptrFrameRateAutoModeOff->GetValue();

    // Set integer value from entry node as new value of enumeration node
    ptrFrameRateAuto->SetIntValue(valueFrameRateAutoOff);

    cout << "AcquisitionFrameRateAuto set to OFF" << endl;

    return true;
}

bool SetFrameRate(CameraPtr pCam)
{
    //-IsMaxFrameRate
    if (UseMaxFramerate)
    {
        try
        {
            EnableManualFramerate(pCam);

            cout << "Setting maximum framerate" << endl;

            INodeMap& nodeMap = pCam->GetNodeMap();

            // Set AcquisitionFrameRate to maximum
            CFloatPtr AcquisitionFrameRateNode = nodeMap.GetNode("AcquisitionFrameRate");
            if (!IsAvailable(AcquisitionFrameRateNode) || !IsWritable(AcquisitionFrameRateNode))
            {
                cout << "Unable to set AcquisitionFrameRate to Max. Aborting..." << endl << endl;
                return false;
            }

            AcquisitionFrameRateNode->SetValue(AcquisitionFrameRateNode->GetMax());
        }
        catch (Spinnaker::Exception& e)
        {
            cout << "Exception setting FrameRate to maximum: " << e.what() << endl;
        }

        return true;
    }

    // User set framerate
    else if (UserSetFramerate > 0)
    {
        try
        {
            EnableManualFramerate(pCam);
            cout << "Setting framerate to: " << UserSetFramerate << endl << endl;

            INodeMap& nodeMap = pCam->GetNodeMap();

            // Set AcquisitionFrameRate to user defined value
            CFloatPtr AcquisitionFrameRateNode = nodeMap.GetNode("AcquisitionFrameRate");
            if (!IsAvailable(AcquisitionFrameRateNode) || !IsWritable(AcquisitionFrameRateNode))
            {
                cout << "Unable to set AcquisitionFrameRate to " << UserSetFramerate << ". Aborting..." << endl << endl;
                return false;
            }

            AcquisitionFrameRateNode->SetValue(UserSetFramerate);
        }
        catch (Spinnaker::Exception& e)
        {
            cout << "Exception setting framerate: " << e.what() << endl;
        }

        return true;
    }

    return false;
}

// This function acts as the body of the example; please see NodeMapInfo example
// for more in-depth comments on setting up cameras.
int RunSingleCamera(CameraPtr pCam)
{
    int result = 0;

    try
    {
        // Retrieve GenTL nodemap and print device information
        INodeMap& nodeMapGenTL = pCam->GetTLDeviceNodeMap();

        result = PrintDeviceInfo(nodeMapGenTL);

        cout << endl << endl << "*** INITIALIZING DEVICE ***" << endl << endl;

        // Initialize camera
        pCam->Init();

        // Retrieve GenICam nodemap
        INodeMap& nodeMap = pCam->GetNodeMap();

        // set the packet size
        try
        {
            cout << "Setting the Packet Size to: " << PacketSizeToSet << endl << endl;
            CIntegerPtr PacketSizeNode = nodeMap.GetNode("GevSCPSPacketSize");
            if (!IsAvailable(PacketSizeNode) || !IsWritable(PacketSizeNode))
            {
                cout << "Unable to set Packet Size to: " << PacketSizeToSet << ". Aborting..." << endl << endl;
            }

            PacketSizeNode->SetValue(PacketSizeToSet);

            cout << "Setting the Packet Delay to: " << PacketDelayToSet << endl << endl;
            CIntegerPtr PacketDelayNode = nodeMap.GetNode("GevSCPD");
            if (!IsAvailable(PacketSizeNode) || !IsWritable(PacketSizeNode))
            {
                cout << "Unable to set Packet Delay to: " << PacketDelayToSet << ". Aborting..." << endl << endl;
            }

            PacketDelayNode->SetValue(PacketDelayToSet);
        }
        catch (Spinnaker::Exception& e)
        {
            cout << "Exception setting packet size to " << PacketSizeToSet << ". Exception: " << e.what() << endl;
        }

        // Set pixel format
        try
        {
            if (PixelFormatToSet)
            {
                cout << "Setting Pixel Format to: " << PixelFormatToSet << endl << endl;
                CEnumerationPtr PixalFormatNode = nodeMap.GetNode("PixelFormat");
                if (!IsAvailable(PixalFormatNode) || !IsReadable(PixalFormatNode))
                {
                    cout << "Unable to read PixalFormat. Aborting..." << endl << endl;
                    return false;
                }

                CEnumEntryPtr PixelFormat_entry = PixalFormatNode->GetEntryByName(PixelFormatToSet);
                if (!IsAvailable(PixalFormatNode) || !IsWritable(PixalFormatNode))
                {
                    cout << "Unable to set PixalFormat to: " << PixelFormatToSet << ". Aborting..." << endl << endl;
                    return false;
                }

                PixalFormatNode->SetIntValue(PixelFormat_entry->GetValue());
            }
        }
        catch (Spinnaker::Exception& e)
        {
            cout << "Exception setting Pixel Format to " << PixelFormatToSet << e.what() << endl;
        }

        // Setting exposure time
        try
        {
            try
            {
                cout << endl << "Turning off ExposureAuto..." << endl;
                CEnumerationPtr ExposureModeNode = nodeMap.GetNode("ExposureAuto");
                if (!IsAvailable(ExposureModeNode) || !IsWritable(ExposureModeNode))
                {
                    cout << "Unable to turn off ExposureAuto. Aborting..." << endl << endl;
                    return false;
                }

                ExposureModeNode->SetIntValue(0);
            }
            catch (Exception e)
            {
                cout << "Exception setting ExposureAuto to Off. Exception: " << e.what() << endl;
            }

            cout << endl << "Setting minimum Exposure Time" << endl;
            CFloatPtr ExposureTimeNode = nodeMap.GetNode("ExposureTime");
            if (!IsAvailable(ExposureTimeNode) || !IsWritable(ExposureTimeNode))
            {
                cout << "Unable to set ExposureTime to minimum. Aborting..." << endl << endl;
                return false;
            }

            ExposureTimeNode->SetValue(ExposureTimeNode->GetMin() + 10);
        }
        catch (Spinnaker::Exception& e)
        {
            cout << "Exception setting minimum exposure time. Exception: " << e.what() << endl;
        }

        // Set FrameRate
        SetFrameRate(pCam);

        if (UseDuration)
        {
            cout << "Streaming for duration of: " << TestDuration << " seconds" << endl;

            // Get FrameRate
            INodeMap& nodeMap = pCam->GetNodeMap();
            CFloatPtr AcquisitionFrameRateNode = nodeMap.GetNode("AcquisitionFrameRate");
            if (!IsAvailable(AcquisitionFrameRateNode) || !IsReadable(AcquisitionFrameRateNode))
            {
                cout << "Unable to read AcquisitionFrameRate. Aborting..." << endl << endl;
                return false;
            }

            double FrameRate = AcquisitionFrameRateNode->GetValue();
            NumImagesToGrab = static_cast<int>(FrameRate * TestDuration);
        }

        // Acquire images
        cout << "This iteration will stream: " << NumImagesToGrab << " images" << endl;
        result = result | AcquireImages(pCam, nodeMap, nodeMapGenTL, NumImagesToGrab, 0);

        // Print Data Stream Nodemap Information
        PrintDataStreamInfo(pCam);

        // Print CPU Usage Stats
        PrintCPUUsage();

        // Deinitialize camera
        pCam->DeInit();
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Error: " << e.what() << endl;
        result = -1;
    }

    return result;
}

// Example entry point; please see Enumeration example for more in-depth
// comments on preparing and cleaning up the system.
int main(int argc, char* argv[])
{
    // Print application build information
    cout << "Application build date: " << __DATE__ << " " << __TIME__ << endl << endl;

    // Parse arguments
    if (!ParseArguments(argc, argv))
    {
        return -1;
    }

    int result = 0;

    // Retrieve singleton reference to system object
    SystemPtr system = System::GetInstance();

    // Print out current library version
    const LibraryVersion spinnakerLibraryVersion = system->GetLibraryVersion();
    cout << "Spinnaker library version: " << spinnakerLibraryVersion.major << "." << spinnakerLibraryVersion.minor
         << "." << spinnakerLibraryVersion.type << "." << spinnakerLibraryVersion.build << endl
         << endl;

    // Retrieve list of cameras from the system
    CameraList camList = system->GetCameras();

    unsigned int numCameras = camList.GetSize();

    cout << "Number of cameras detected: " << numCameras << endl << endl;

    // Finish if there are no cameras
    if (numCameras == 0)
    {
        // Clear camera list before releasing system
        camList.Clear();

        // Release system
        system->ReleaseInstance();

        cout << "No cameras detected." << endl;
        cout << "Done!" << endl;
        // getchar();

        return -1;
    }

    //
    // Create shared pointer to camera
    //
    // *** NOTES ***
    // The CameraPtr object is a shared pointer, and will generally clean itself
    // up upon exiting its scope. However, if a shared pointer is created in the
    // same scope that a system object is explicitly released (i.e. this scope),
    // the reference to the shared point must be broken manually.
    //
    // *** LATER ***
    // Shared pointers can be terminated manually by assigning them to nullptr.
    // This keeps releasing the system from throwing an exception.
    //
    CameraPtr pCam = nullptr;

    // Run example on each camera
    for (unsigned int i = 0; i < numCameras; i++)
    {
        // Select camera
        pCam = camList.GetByIndex(i);

        cout << endl << "Running code for camera " << i << "..." << endl;
        // Run example
        result = result | RunSingleCamera(pCam);

        cout << "Camera " << i << " complete..." << endl << endl;
    }

    //
    // Release reference to the camera
    //
    // *** NOTES ***
    // Had the CameraPtr object been created within the for-loop, it would not
    // be necessary to manually break the reference because the shared pointer
    // would have automatically cleaned itself up upon exiting the loop.
    //
    pCam = nullptr;

    // Clear camera list before releasing system
    camList.Clear();

    // Release system
    system->ReleaseInstance();

    cout << endl << "Done! Press Enter to exit..." << endl;
    getchar();

    return result;
}
