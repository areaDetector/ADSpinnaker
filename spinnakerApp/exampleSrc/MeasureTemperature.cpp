 
#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
using namespace std;

#ifdef LINUX
  #define Sleep(MS) sleep(MS/1000)
#endif

#define DEFAULT_TEMPERATURE_LOOPS 5

int main(int argc, char** argv)
{
  // Program takes 2 optional arguments
  // Serial number of camera.  If this argument is absent it uses the first camera in the system.
  // Number of times to measure temperature.  If this argument is absent it defaults to 5 times.
  
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
    
    int numLoops = 5;
    if (argc > 2) {
        numLoops = atoi(argv[2]);
    }
  
  	// Initialize camera
  	pCam->Init();
  		
  	// Retrieve GenICam nodemap
  	INodeMap & nodeMap = pCam->GetNodeMap();
    CStringPtr pStringNode = (CStringPtr)nodeMap.GetNode("DeviceModelName");
    cout << "Model name: " << pStringNode->GetValue() << endl;
    pStringNode = (CStringPtr)nodeMap.GetNode("DeviceSerialNumber");
    cout << "Serial number: " << pStringNode->GetValue() << endl;
    CFloatPtr pNode = (CFloatPtr)nodeMap.GetNode("DeviceTemperature");
  
  	for (int i=0; i<numLoops; i++) {
  	     printf("Loop = %d, Temperature = %f\n", i, pNode->GetValue());
  	     Sleep(2000);
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
