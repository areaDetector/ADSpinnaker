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
 */

#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"
#include <iostream>
#include <sstream>

// Total number of buffers
#define numBuffers 3

// Number of triggers
#define z_numTriggers 6

// Total number of loops
#define k_numLoops 9

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

// This function configures the camera to use a trigger. First, trigger mode is
// set to off in order to select the trigger source. Once the trigger source
// has been selected, trigger mode is then enabled, which has the camera
// capture only a single image upon the execution of the trigger.
int ConfigureTrigger(INodeMap& nodeMap)
{
    int result = 0;

    cout << endl << endl << "*** CONFIGURING TRIGGER ***" << endl << endl;

    try
    {
        //
        // Ensure trigger mode off
        //
        // *** NOTES ***
        // The trigger must be disabled in order to configure the
        // trigger source.
        //
        CEnumerationPtr ptrTriggerMode = nodeMap.GetNode("TriggerMode");
        if (!IsAvailable(ptrTriggerMode) || !IsWritable(ptrTriggerMode))
        {
            cout << "Unable to disable trigger mode (node retrieval). Aborting..." << endl;
            return -1;
        }

        CEnumEntryPtr ptrTriggerModeOff = ptrTriggerMode->GetEntryByName("Off");
        if (!IsAvailable(ptrTriggerModeOff) || !IsReadable(ptrTriggerModeOff))
        {
            cout << "Unable to disable trigger mode (enum entry retrieval). Aborting..." << endl;
            return -1;
        }

        ptrTriggerMode->SetIntValue(ptrTriggerModeOff->GetValue());

        cout << endl << "Trigger mode disabled..." << endl;

        // Set trigger source to software
        CEnumerationPtr ptrTriggerSource = nodeMap.GetNode("TriggerSource");
        if (!IsAvailable(ptrTriggerSource) || !IsWritable(ptrTriggerSource))
        {
            cout << "Unable to set trigger mode (node retrieval). Aborting..." << endl;
            return -1;
        }

        CEnumEntryPtr ptrTriggerSourceSoftware = ptrTriggerSource->GetEntryByName("Software");
        if (!IsAvailable(ptrTriggerSourceSoftware) || !IsReadable(ptrTriggerSourceSoftware))
        {
            cout << "Unable to set trigger mode (enum entry retrieval). Aborting..." << endl;
            return -1;
        }

        ptrTriggerSource->SetIntValue(ptrTriggerSourceSoftware->GetValue());

        cout << "Trigger source set to software..." << endl;

        // Turn trigger mode on
        CEnumEntryPtr ptrTriggerModeOn = ptrTriggerMode->GetEntryByName("On");
        if (!IsAvailable(ptrTriggerModeOn) || !IsReadable(ptrTriggerModeOn))
        {
            cout << "Unable to enable trigger mode (enum entry retrieval). Aborting..." << endl;
            return -1;
        }
        ptrTriggerMode->SetIntValue(ptrTriggerModeOn->GetValue());

        cout << "Trigger mode turned back on..." << endl << endl;
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
        if (!IsAvailable(ptrSoftwareTriggerCommand) || !IsWritable(ptrSoftwareTriggerCommand))
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
        if (!IsAvailable(ptrTriggerMode) || !IsWritable(ptrTriggerMode))
        {
            cout << "Unable to disable trigger mode (node retrieval). Non-fatal error..." << endl;
            return -1;
        }

        CEnumEntryPtr ptrTriggerModeOff = ptrTriggerMode->GetEntryByName("Off");
        if (!IsAvailable(ptrTriggerModeOff) || !IsReadable(ptrTriggerModeOff))
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
        if (IsAvailable(category) && IsReadable(category))
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

// This function cycles through the four different buffer handling modes.
// It saves three images for three of the buffer handling modes
// (NewestFirst, OldestFirst, and OldestFirstOverwrite).  For NewestOnly,
// it saves one image.
int AcquireImages(CameraPtr pCam, INodeMap& nodeMap, INodeMap& nodeMapTLDevice)
{
    int result = 0;

    cout << endl << "*** IMAGE ACQUISITION ***" << endl << endl;

    try
    {
        // Set acquisition mode to continuous
        CEnumerationPtr ptrAcquisitionMode = nodeMap.GetNode("AcquisitionMode");
        if (!IsAvailable(ptrAcquisitionMode) || !IsWritable(ptrAcquisitionMode))
        {
            cout << "Unable to set acquisition mode to continuous (node retrieval). Aborting..." << endl << endl;
            return -1;
        }

        CEnumEntryPtr ptrAcquisitionModeContinuous = ptrAcquisitionMode->GetEntryByName("Continuous");
        if (!IsAvailable(ptrAcquisitionModeContinuous) || !IsReadable(ptrAcquisitionModeContinuous))
        {
            cout << "Unable to set acquisition mode to continuous (entry 'continuous' retrieval). Aborting..." << endl
                 << endl;
            return -1;
        }

        ptrAcquisitionMode->SetIntValue(ptrAcquisitionModeContinuous->GetValue());

        cout << "Acquisition mode set to continuous..." << endl;

        // Retrieve device serial number for filename
        gcstring deviceSerialNumber("");

        CStringPtr ptrStringSerial = nodeMapTLDevice.GetNode("DeviceSerialNumber");
        if (IsAvailable(ptrStringSerial) && IsReadable(ptrStringSerial))
        {
            deviceSerialNumber = ptrStringSerial->GetValue();

            cout << "Device serial number retrieved as " << deviceSerialNumber << "..." << endl;
        }

        // Retrieve Stream Parameters device nodemap
        Spinnaker::GenApi::INodeMap& sNodeMap = pCam->GetTLStreamNodeMap();

        // Retrieve Buffer Handling Mode Information
        CEnumerationPtr ptrHandlingMode = sNodeMap.GetNode("StreamBufferHandlingMode");
        if (!IsAvailable(ptrHandlingMode) || !IsWritable(ptrHandlingMode))
        {
            cout << "Unable to set Buffer Handling mode (node retrieval). Aborting..." << endl << endl;
            return -1;
        }

        CEnumEntryPtr ptrHandlingModeEntry = ptrHandlingMode->GetCurrentEntry();
        if (!IsAvailable(ptrHandlingModeEntry) || !IsReadable(ptrHandlingModeEntry))
        {
            cout << "Unable to set Buffer Handling mode (Entry retrieval). Aborting..." << endl << endl;
            return -1;
        }

        // Set stream buffer Count Mode to manual
        CEnumerationPtr ptrStreamBufferCountMode = sNodeMap.GetNode("StreamBufferCountMode");
        if (!IsAvailable(ptrStreamBufferCountMode) || !IsWritable(ptrStreamBufferCountMode))
        {
            cout << "Unable to set Buffer Count Mode (node retrieval). Aborting..." << endl << endl;
            return -1;
        }

        CEnumEntryPtr ptrStreamBufferCountModeManual = ptrStreamBufferCountMode->GetEntryByName("Manual");
        if (!IsAvailable(ptrStreamBufferCountModeManual) || !IsReadable(ptrStreamBufferCountModeManual))
        {
            cout << "Unable to set Buffer Count Mode entry (Entry retrieval). Aborting..." << endl << endl;
            return -1;
        }

        ptrStreamBufferCountMode->SetIntValue(ptrStreamBufferCountModeManual->GetValue());

        cout << "Stream Buffer Count Mode set to manual..." << endl;

        // Retrieve and modify Stream Buffer Count
        CIntegerPtr ptrBufferCount = sNodeMap.GetNode("StreamBufferCountManual");
        if (!IsAvailable(ptrBufferCount) || !IsWritable(ptrBufferCount))
        {
            cout << "Unable to set Buffer Count (Integer node retrieval). Aborting..." << endl << endl;
            return -1;
        }

        // Display Buffer Info
        cout << endl << "Default Buffer Handling Mode: " << ptrHandlingModeEntry->GetDisplayName() << endl;
        cout << "Default Buffer Count: " << ptrBufferCount->GetValue() << endl;
        cout << "Maximum Buffer Count: " << ptrBufferCount->GetMax() << endl;

        ptrBufferCount->SetValue(numBuffers);

        cout << "Buffer count now set to: " << ptrBufferCount->GetValue() << endl;

        cout << endl
             << "Camera will be triggered " << z_numTriggers << " times in a row before " << k_numLoops - z_numTriggers
             << " images will be retrieved" << endl;

        for (int i = 0; i < 4; i++)
        {
            switch (i)
            {
            case 0:
                ptrHandlingModeEntry = ptrHandlingMode->GetEntryByName("NewestFirst");
                ptrHandlingMode->SetIntValue(ptrHandlingModeEntry->GetValue());
                cout << endl
                     << endl
                     << "Buffer Handling Mode has been set to " << ptrHandlingModeEntry->GetDisplayName() << endl;
                break;

            case 1:
                ptrHandlingModeEntry = ptrHandlingMode->GetEntryByName("NewestOnly");
                ptrHandlingMode->SetIntValue(ptrHandlingModeEntry->GetValue());
                cout << endl
                     << endl
                     << "Buffer Handling Mode has been set to " << ptrHandlingModeEntry->GetDisplayName() << endl;
                break;

            case 2:
                ptrHandlingModeEntry = ptrHandlingMode->GetEntryByName("OldestFirst");
                ptrHandlingMode->SetIntValue(ptrHandlingModeEntry->GetValue());
                cout << endl
                     << endl
                     << "Buffer Handling Mode has been set to " << ptrHandlingModeEntry->GetDisplayName() << endl;
                break;

            case 3:
                ptrHandlingModeEntry = ptrHandlingMode->GetEntryByName("OldestFirstOverwrite");
                ptrHandlingMode->SetIntValue(ptrHandlingModeEntry->GetValue());
                cout << endl
                     << endl
                     << "Buffer Handling Mode has been set to " << ptrHandlingModeEntry->GetDisplayName() << endl;
                break;

            default:
                break;
            }

            // Begin capturing images
            pCam->BeginAcquisition();

            // Sleep for one second; only necessary when using non-BFS/ORX cameras on startup
            if (i == 0)
            {
                SleepyWrapper(1000);
            }

            try
            {
                // Software Trigger the camera then  save images
                for (int loopCnt = 0; loopCnt < k_numLoops; loopCnt++)
                {
                    ImagePtr pResultImage;
                    if (loopCnt < z_numTriggers)
                    {
                        // Retrieve the next image from the trigger
                        result = result | GrabNextImageByTrigger(nodeMap);
                        cout << endl << "Camera triggered. No image grabbed" << endl;
                    }
                    else
                    {
                        cout << endl << "No trigger. Grabbing image " << loopCnt - z_numTriggers << endl;

                        pResultImage = pCam->GetNextImage(500);
                        if (pResultImage->IsIncomplete())
                        {
                            cout << "Image incomplete with image status " << pResultImage->GetImageStatus() << "..."
                                 << endl
                                 << endl;
                        }
                    }

                    if (loopCnt >= z_numTriggers)
                    {
                        // Retrieve Frame ID
                        cout << "Frame ID: " << pResultImage->GetFrameID() << endl;

                        // Create a unique filename
                        ostringstream filename;

                        filename << ptrHandlingModeEntry->GetSymbolic() << "-";

                        if (!deviceSerialNumber.empty())
                        {
                            filename << deviceSerialNumber.c_str() << "-";
                        }

                        filename << loopCnt - z_numTriggers << ".jpg";

                        // Save image
                        pResultImage->Save(filename.str().c_str());
                        cout << "Image saved at " << filename.str() << endl;

                        // Release image
                        pResultImage->Release();
                    }

                    // To control the framerate, have the application pause for 250ms.
                    SleepyWrapper(250);
                }
            }
            catch (Spinnaker::Exception& e)
            {
                cout << endl << "Error: " << e.what() << endl;

                if (ptrHandlingModeEntry->GetSymbolic() == "NewestOnly")
                {
                    cout << "Error should occur when grabbing image 1 with handling mode set to NewestOnly" << endl;
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
int RunSingleCamera(CameraPtr pCam)
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
        cout << "Press Enter to exit..." << endl;
        getchar();
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
        cout << "Done! Press Enter to exit..." << endl;
        getchar();

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

    cout << endl << "Done! Press Enter to exit..." << endl;
    getchar();

    return result;
}
