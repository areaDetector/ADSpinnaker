 
#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
using namespace std;


int main(int argc, char** argv)
{
  // argv[1]: Serial number of camera.  If this argument is absent it uses the first camera in the system.
  
  int result = 0;
	// Retrieve singleton reference to system object
	SystemPtr system = System::GetInstance();

	// Retrieve list of cameras from the system
	CameraList camList = system->GetCameras();

	CameraPtr pCam = NULL;
  
	try {
    // Select camera
    if (argc > 1) {
      std::string serialNumber = argv[1];
      cout << "Opening camera serial number: " << serialNumber << endl;
  	  pCam = camList.GetBySerial(serialNumber);
    } else {
      cout << "Opening camera index 0" << endl;
      pCam = camList.GetByIndex(0);
    }
    
  	// Initialize camera
  	pCam->Init();
  		
  	// Retrieve GenICam nodemap
  	INodeMap & nodeMap = pCam->GetNodeMap();

    CStringPtr pStringNode;
    
    pStringNode = (CStringPtr)nodeMap.GetNode("DeviceModelName");
    cout << "Model name: " << pStringNode->GetValue() << endl;
    pStringNode = (CStringPtr)nodeMap.GetNode("DeviceFirmwareVersion");
    cout << "Firmware version: " << pStringNode->GetValue() << endl;
    pStringNode = (CStringPtr)nodeMap.GetNode("DeviceSerialNumber");
    cout << "Serial number: " << pStringNode->GetValue() << endl;

    CEnumerationPtr pPixelFormatNode = nodeMap.GetNode("PixelFormat");
    CFloatPtr pFrameRateNode = nodeMap.GetNode("AcquisitionFrameRate");
    CBooleanPtr pColorProcessNode = nodeMap.GetNode("OnBoardColorProcessEnabled");

   // Loop over the available pixel formats, set the format, and then read the maximum frame rate
    NodeList_t entries;
    pPixelFormatNode->GetEntries(entries);
    int numEnums = (int)entries.size();

    for (int i=0; i<numEnums; i++) {
        IEnumEntry *pEntry= dynamic_cast<IEnumEntry *>(entries[i]);
        if (IsAvailable(pEntry) && IsReadable(pEntry)) {
            cout  << endl << "Setting PixelFormat: " << pEntry->GetSymbolic() << endl;
            long long entryValue = pEntry->GetValue();
            pPixelFormatNode->SetIntValue(entryValue);
            if (IsWritable(pColorProcessNode)) {
                cout << "Setting OnBoardColorProcessEnabled to false" << endl;
                *pColorProcessNode = false;
            } else {
                cout << "Error: OnBoardColorProcessEnabled is not writeable" << endl;
            }
            double maxFrameRate = pFrameRateNode->GetMax();
            cout << "Maximum frame rate: " << maxFrameRate << endl;
        }
    }
      		
	}
	catch (Spinnaker::Exception &e)
	{
		cout << "Error: " << e.what() << endl;
		result = -1;
	}

	pCam = NULL;

	// Clear camera list before releasing system
	camList.Clear();

	// Release system
	system->ReleaseInstance();

  return result;
}
