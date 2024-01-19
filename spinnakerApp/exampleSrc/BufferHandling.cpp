//=============================================================================
// Copyright (c) 2001-2023 FLIR Systems, Inc. All Rights Reserved.
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
 *  @example BufferHandling.cpp
 *
 *  @brief BufferHandling.cpp shows how the different buffer handling modes work.
 *  It relies on information provided in the Acquisition and Trigger examples.
 *
 *  Buffer handling determines the ordering in which images are retrieved, and
 *  what occurs when an image is transmitted while the buffer is full.  There are
 *  four different buffer handling modes available; NewestFirst, NewestOnly,
 *  OldestFirst and OldestFirstOverwrite.
 *
 *  This example explores retrieving images in a set pattern; triggering the camera
 *  while not retrieving an image (letting the buffer fill up), and retrieving
 *  images while not triggering.  We cycle through the different buffer handling
 *  modes to see which images are retrieved, confirming their identities via their
 *  Frame ID values.
 *
 *  Please leave us feedback at: https://www.surveymonkey.com/r/TDYMVAPI
 *  More source code examples at: https://github.com/Teledyne-MV/Spinnaker-Examples
 *  Need help? Check out our forum at: https://teledynevisionsolutions.zendesk.com/hc/en-us/community/topics
 */

#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"
#include <iostream>
#include <sstream>

// Total number of GenTL buffers. 1-2 buffers unavailable for some buffer modes
constexpr int numBuffers = 6;

// Number of triggers to load images from camera to Spinnaker
constexpr int numTriggers = 10;

// Number of times attempted to grab an image from Spinnaker to application
constexpr int numGrabs = 10;

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
using namespace std;

// This helper function allows the example to sleep in both Windows and Linux
// systems. Note that Windows sleep takes milliseconds as a parameter while
// Linux systems take microseconds as a parameter.
void SleepyWrapper(int milliseconds)
{
#if defined WIN32 || defined _WIN32 || defined WIN64 || defined _WIN64
    Sleep(milliseconds);
#else
    usleep(1000 * milliseconds);
#endif
}

// This helper function determines the appropriate number of images to expect
// when running this example on various cameras and stream modes.
int GetExpectedImageCount(INodeMap& nodeMapTLDevice, INodeMap& snodeMap)
{
    // Check DeviceType and only adjust count for GigEVision device
    CEnumerationPtr ptrDeviceType = nodeMapTLDevice.GetNode("DeviceType");
    if (IsReadable(ptrDeviceType) && ptrDeviceType->GetIntValue() == DeviceType_GigEVision)
    {
        // Check StreamMode
        CEnumerationPtr ptrStreamMode = snodeMap.GetNode("StreamMode");
        if (!IsAvailable(ptrStreamMode) || !IsReadable(ptrStreamMode))
        {
            cout << "Unable to get device's stream mode. Aborting..." << endl << endl;
            return -1;
        }

        // Adjust the expected image count to account for the trash buffer in
        // TeledyneGigeVision driver, where we expect one less image than the
        // total number of buffers
        if (ptrStreamMode->GetIntValue() == StreamMode_TeledyneGigeVision)
        {
            return (numBuffers - 1);
        }
    }

    return numBuffers;
}

// This function configures the camera to use a trigger. First, trigger mode is
// set to off in order to select the trigger source. Once the trigger source
// has been selected, trigger mode is then enabled, which has the camera
// capture only a single image upon the execution of the trigger.
int ConfigureTrigger(INodeMap& nodeMap)
{
    int result = 0;

    cout << endl << "*** CONFIGURING TRIGGER ***" << endl;

    try
    {
        //
        // Ensure trigger mode on
        //
        // *** NOTES ***
        // The trigger must be enabled in order to configure the
        // trigger source.
        //
        CEnumerationPtr ptrTriggerMode = nodeMap.GetNode("TriggerMode");
        if (!IsReadable(ptrTriggerMode) || !IsWritable(ptrTriggerMode))
        {
            cout << "Unable to enable trigger mode (node retrieval). Aborting..." << endl;
            return -1;
        }

        CEnumEntryPtr ptrTriggerModeOn = ptrTriggerMode->GetEntryByName("On");
        if (!IsReadable(ptrTriggerModeOn))
        {
            cout << "Unable to enable trigger mode (enum entry retrieval). Aborting..." << endl;
            return -1;
        }

        ptrTriggerMode->SetIntValue(ptrTriggerModeOn->GetValue());

        cout << endl << "Trigger mode enabled..." << endl;

        // Set trigger source to software
        CEnumerationPtr ptrTriggerSource = nodeMap.GetNode("TriggerSource");
        if (!IsReadable(ptrTriggerSource) || !IsWritable(ptrTriggerSource))
        {
            cout << "Unable to get or set trigger mode (node retrieval). Aborting..." << endl;
            return -1;
        }

        CEnumEntryPtr ptrTriggerSourceSoftware = ptrTriggerSource->GetEntryByName("Software");
        if (!IsReadable(ptrTriggerSourceSoftware))
        {
            cout << "Unable to set trigger mode (enum entry retrieval). Aborting..." << endl;
            return -1;
        }

        ptrTriggerSource->SetIntValue(ptrTriggerSourceSoftware->GetValue());

        cout << "Trigger source set to software..." << endl;
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Error: " << e.what() << endl;
        result = -1;
    }

    return result;
}

// This function retrieves a single image using the trigger. In this example,
// only a single image is captured and made available for acquisition - as such,
// attempting to acquire two images for a single trigger execution would cause
// the example to hang. This is different from other examples, whereby a
// constant stream of images are being captured and made available for image
// acquisition.
int GrabNextImageByTrigger(INodeMap& nodeMap)
{
    int result = 0;

    try
    {
        // Execute software trigger
        CCommandPtr ptrSoftwareTriggerCommand = nodeMap.GetNode("TriggerSoftware");
        if (!IsWritable(ptrSoftwareTriggerCommand))
        {
            cout << "Unable to execute trigger. Aborting..." << endl;
            return -1;
        }

        ptrSoftwareTriggerCommand->Execute();
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Error: " << e.what() << endl;
        result = -1;
    }

    return result;
}

// This function returns the camera to a normal state by turning off trigger
// mode.
int ResetTrigger(INodeMap& nodeMap)
{
    int result = 0;

    try
    {
        //
        // Turn trigger mode back off
        //
        // *** NOTES ***
        // Once all images have been captured, turn trigger mode back off to
        // restore the camera to a clean state.
        //
        CEnumerationPtr ptrTriggerMode = nodeMap.GetNode("TriggerMode");
        if (!IsReadable(ptrTriggerMode) || !IsWritable(ptrTriggerMode))
        {
            cout << "Unable to disable trigger mode (node retrieval). Non-fatal error..." << endl;
            return -1;
        }

        CEnumEntryPtr ptrTriggerModeOff = ptrTriggerMode->GetEntryByName("Off");
        if (!IsReadable(ptrTriggerModeOff))
        {
            cout << "Unable to disable trigger mode (enum entry retrieval). Non-fatal error..." << endl;
            return -1;
        }

        ptrTriggerMode->SetIntValue(ptrTriggerModeOff->GetValue());

        cout << endl << endl << "Trigger mode disabled..." << endl << endl;
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
        // Retrieve and display Device Information
        CCategoryPtr category = nodeMap.GetNode("DeviceInformation");
        if (IsReadable(category))
        {
            FeatureList_t features;
            category->GetFeatures(features);

            for (FeatureList_t::const_iterator it = features.begin(); it != features.end(); ++it)
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
            cout << "Device control information not readable." << endl;
        }
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Error: " << e.what() << endl;
        result = -1;
    }

    return result;
}

// This function cycles through the four different buffer handling modes.
// It saves three images for three of the buffer handling modes
// (NewestFirst, OldestFirst, and OldestFirstOverwrite).  For NewestOnly,
// it saves one image.
int AcquireImages(const CameraPtr& pCam, INodeMap& nodeMap, INodeMap& nodeMapTLDevice)
{
    int result = 0;

    cout << endl << "*** IMAGE ACQUISITION ***" << endl << endl;

    try
    {
        // Set acquisition mode to continuous
        CEnumerationPtr ptrAcquisitionMode = nodeMap.GetNode("AcquisitionMode");
        if (!IsReadable(ptrAcquisitionMode) || !IsWritable(ptrAcquisitionMode))
        {
            cout << "Unable to set acquisition mode to continuous (node retrieval). Aborting..." << endl << endl;
            return -1;
        }

        CEnumEntryPtr ptrAcquisitionModeContinuous = ptrAcquisitionMode->GetEntryByName("Continuous");
        if (!IsReadable(ptrAcquisitionModeContinuous))
        {
            cout << "Unable to set acquisition mode to continuous (entry 'continuous' retrieval). Aborting..." << endl
                 << endl;
            return -1;
        }

        ptrAcquisitionMode->SetIntValue(ptrAcquisitionModeContinuous->GetValue());

        cout << "Acquisition mode set to continuous..." << endl;

        // Set pixel format to mono8
        CEnumerationPtr ptrPixelFormat = nodeMap.GetNode("PixelFormat");

        if (!IsWritable(ptrPixelFormat))
        {
            cout << "Unable to set Pixel Format mode (node retrieval). Aborting..." << endl << endl;
            return false;
        }
        CEnumEntryPtr ptrMono8 = ptrPixelFormat->GetEntryByName("Mono8");
        if (!IsReadable(ptrMono8))
        {
            cout << "Unable to set pixel format (entry 'mono8' retrieval). Aborting..." << endl << endl;
            return false;
        }
        ptrPixelFormat->SetIntValue(ptrMono8->GetValue());
        cout << "Pixel format set to " << ptrPixelFormat->GetCurrentEntry()->GetName() << endl;

        // Retrieve device serial number for filename
        gcstring deviceSerialNumber("");

        CStringPtr ptrStringSerial = nodeMapTLDevice.GetNode("DeviceSerialNumber");
        if (IsReadable(ptrStringSerial))
        {
            deviceSerialNumber = ptrStringSerial->GetValue();

            cout << "Device serial number retrieved as " << deviceSerialNumber << "..." << endl;
        }

        // Retrieve Stream Parameters device nodemap
        Spinnaker::GenApi::INodeMap& sNodeMap = pCam->GetTLStreamNodeMap();

        // Retrieve Buffer Handling Mode Information
        CEnumerationPtr ptrHandlingMode = sNodeMap.GetNode("StreamBufferHandlingMode");
        if (!IsReadable(ptrHandlingMode) || !IsWritable(ptrHandlingMode))
        {
            cout << "Unable to set Buffer Handling mode (node retrieval). Aborting..." << endl << endl;
            return -1;
        }

        CEnumEntryPtr ptrHandlingModeEntry = ptrHandlingMode->GetCurrentEntry();

        if (!IsReadable(ptrHandlingModeEntry))
        {
            cout << "Unable to get Buffer Handling mode (Entry retrieval). Aborting..." << endl << endl;
            return -1;
        }

        // Set stream buffer Count Mode to manual
        CEnumerationPtr ptrStreamBufferCountMode = sNodeMap.GetNode("StreamBufferCountMode");
        if (!IsReadable(ptrStreamBufferCountMode) || !IsWritable(ptrStreamBufferCountMode))
        {
            cout << "Unable to get or set Buffer Count Mode (node retrieval). Aborting..." << endl << endl;
            return -1;
        }

        CEnumEntryPtr ptrStreamBufferCountModeManual = ptrStreamBufferCountMode->GetEntryByName("Manual");
        if (!IsReadable(ptrStreamBufferCountModeManual))
        {
            cout << "Unable to get Buffer Count Mode entry (Entry retrieval). Aborting..." << endl << endl;
            return -1;
        }

        ptrStreamBufferCountMode->SetIntValue(ptrStreamBufferCountModeManual->GetValue());

        cout << "Stream Buffer Count Mode set to manual..." << endl;

        // Retrieve and modify Stream Buffer Count
        CIntegerPtr ptrBufferCount = sNodeMap.GetNode("StreamBufferCountManual");
        if (!IsReadable(ptrBufferCount) || !IsWritable(ptrBufferCount))
        {
            cout << "Unable to get or set Buffer Count (Integer node retrieval). Aborting..." << endl << endl;
            return -1;
        }

        // Display Buffer Info
        cout << endl << "Default Buffer Handling Mode: " << ptrHandlingModeEntry->GetDisplayName() << endl;
        cout << "Default Buffer Count: " << ptrBufferCount->GetValue() << endl;
        cout << "Maximum Buffer Count: " << ptrBufferCount->GetMax() << endl;

        ptrBufferCount->SetValue(numBuffers);

        cout << "Buffer count now set to: " << ptrBufferCount->GetValue() << endl;

        cout << endl
             << "Camera will be triggered " << numTriggers << " times in a row, followed by " << numGrabs
             << " image retrieval attempts" << endl;
        cout << endl << "Note - Buffer behaviour is different for USB3 and GigE cameras" << endl;
        cout << "     - USB3 cameras buffer images internally if no host buffers are available" << endl;
        cout << "     - Once the stream buffer is released, the USB3 camera will fill that buffer" << endl;
        cout << "     - GigE cameras do not buffer images" << endl;
        cout << "     - In TeledyneGigEVision stream mode an extra buffer will be reserved for trashing" << endl;

        const std::vector<gcstring> bufferHandlingModes = {
            "NewestFirst", "OldestFirst", "NewestOnly", "OldestFirstOverwrite"};
        for (unsigned int i = 0; i < bufferHandlingModes.size(); i++)
        {
            ptrHandlingModeEntry = ptrHandlingMode->GetEntryByName(bufferHandlingModes[i]);
            ptrHandlingMode->SetIntValue(ptrHandlingModeEntry->GetValue());
            const std::string bufferModeName = ptrHandlingMode->GetCurrentEntry()->GetDisplayName().c_str();
            cout << endl << endl << "*** Buffer Handling Mode has been set to " << bufferModeName << " ***" << endl;

            // Begin capturing images
            pCam->BeginAcquisition();

            // Sleep for one second; only necessary when using non-BFS/ORX cameras on startup
            if (i == 0)
            {
                SleepyWrapper(1000);
            }

            try
            {
                for (int j = 0; j < numTriggers; j++)
                {
                    // Retrieve the next image from the trigger
                    result = result | GrabNextImageByTrigger(nodeMap);

                    // Control framerate
                    SleepyWrapper(250);
                }

                cout << endl << "Camera triggered " << numTriggers << " times" << endl;

                cout << endl << "Retrieving images from library until no image data is returned (errors out)" << endl;

                // Software Trigger the camera then save images
                for (int j = 1; j < numGrabs; j++)
                {
                    // Create a unique filename
                    ostringstream filename;
                    filename << ptrHandlingModeEntry->GetSymbolic() << "-";
                    if (!deviceSerialNumber.empty())
                    {
                        filename << deviceSerialNumber.c_str() << "-";
                    }
                    filename << j << ".jpg";

                    const ImagePtr pResultImage = pCam->GetNextImage(500);
                    if (pResultImage->IsIncomplete())
                    {
                        cout << "Image #" << j << " incomplete with image status " << pResultImage->GetImageStatus()
                             << "..." << endl
                             << endl;
                    }

                    // Retrieve and Save image
                    pResultImage->Save(filename.str().c_str());
                    cout << "GetNextImage() #" << j << ", Frame ID: " << pResultImage->GetFrameID()
                         << ", Image saved at " << filename.str() << endl;

                    // Release image
                    pResultImage->Release();
                }
            }
            catch (Spinnaker::Exception& e)
            {
                cout << "Error: " << e.what() << endl << endl;
                if (ptrHandlingModeEntry->GetSymbolic() == "NewestFirst" ||
                    ptrHandlingModeEntry->GetSymbolic() == "OldestFirst")
                {
                    // In this mode, one buffer is used to cycle images within spinnaker acquisition engine.
                    // Only numBuffers - 1 images will be stored in the library; additional triggered images will be
                    // dropped.
                    // Calling GetNextImage() more than buffered images will return an error.
                    // Note: These two modes differ in the order of images returned.
                    const unsigned int expectedImageCount = GetExpectedImageCount(nodeMapTLDevice, sNodeMap);
                    cout << endl << "EXPECTED: error getting image # " << expectedImageCount + 1
                         << " with handling mode set "
                            "to NewestFirst or OldestFirst in GigE Streaming"
                         << endl;
                }
                else if (ptrHandlingModeEntry->GetSymbolic() == "NewestOnly")
                {
                    // In this mode, a single buffer is overwritten if not read out in time
                    cout << endl << "EXPECTED: error occur when getting image #2 with handling mode set to NewestOnly" << endl;
                }
                if (ptrHandlingModeEntry->GetSymbolic() == "OldestFirstOverwrite")
                {
                    // In this mode, two buffers are used to cycle images within
                    // the spinnaker acquisition engine. Only numBuffers - 2 images will return to the user.
                    // Calling GetNextImage() without additional triggers will return an error
                    cout << endl << "EXPECTED: error occur when getting image #" << numBuffers - 1
                         << " with handling mode set to"
                            " OldestFirstOverwrite"
                         << endl;
                }

                result = -1;
            }

            // End acquisition
            pCam->EndAcquisition();
        }
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Error: " << e.what() << endl;
        result = -1;
    }

    return result;
}

// This function acts as the body of the example; please see NodeMapInfo example
// for more in-depth comments on setting up cameras.
int RunSingleCamera(const CameraPtr& pCam)
{
    int result = 0;
    int err = 0;

    try
    {
        // Retrieve TL device nodemap and print device information
        INodeMap& nodeMapTLDevice = pCam->GetTLDeviceNodeMap();

        result = PrintDeviceInfo(nodeMapTLDevice);

        // Initialize camera
        pCam->Init();

        // Retrieve GenICam nodemap
        INodeMap& nodeMap = pCam->GetNodeMap();

        // Configure trigger
        err = ConfigureTrigger(nodeMap);
        if (err < 0)
        {
            return err;
        }

        // Acquire images
        result = result | AcquireImages(pCam, nodeMap, nodeMapTLDevice);

        // Reset trigger
        result = result | ResetTrigger(nodeMap);

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
int main(int /*argc*/, char** /*argv*/)
{
    // Since this application saves images in the current folder
    // we must ensure that we have permission to write to this folder.
    // If we do not have permission, fail right away.
    FILE* tempFile = fopen("test.txt", "w+");
    if (tempFile == nullptr)
    {
        cout << "Failed to create file in current folder.  Please check "
                "permissions."
             << endl;
        cout << "Press any key to exit..." << endl;
        cin.ignore();
        return -1;
    }
    fclose(tempFile);
    remove("test.txt");

    int result = 0;

    // Print application build information
    cout << "Application build date: " << __DATE__ << " " << __TIME__ << endl << endl;

    // Retrieve singleton reference to system object
    SystemPtr system = System::GetInstance();

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

        cout << "Not enough cameras!" << endl;
        cout << "Done! Press any key to exit..." << endl;
        cin.ignore();

        return -1;
    }

    // Run example on each camera
    for (unsigned int i = 0; i < numCameras; i++)
    {
        cout << endl << "Running example for camera " << i << "..." << endl;

        result = result | RunSingleCamera(camList.GetByIndex(i));

        cout << "Camera " << i << " example complete..." << endl << endl;
    }

    // Clear camera list before releasing system
    camList.Clear();

    // Release system
    system->ReleaseInstance();

    cout << endl << "Done! Press any key to exit..." << endl;
    cin.ignore();

    return result;
}
