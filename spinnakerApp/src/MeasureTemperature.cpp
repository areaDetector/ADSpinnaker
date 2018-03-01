 
#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
using namespace std;

#ifdef Linux
  #define Sleep(MS) sleep(MS/1000)
#endif

#define DEFAULT_SERIAL_NUMBER "17165235"

int main(int argc, char** argv)
{
  int result;
	// Retrieve singleton reference to system object
	SystemPtr system = System::GetInstance();

	// Retrieve list of cameras from the system
	CameraList camList = system->GetCameras();

	CameraPtr pCam = NULL;
	const char *serialNumber = DEFAULT_SERIAL_NUMBER;
  
	try {
    // Select camera
    if (argc > 1) {
      serialNumber = argv[1];
    }
    printf("Serial number=%s\n", serialNumber);
  	pCam = camList.GetBySerial(serialNumber);
  
  	// Initialize camera
  	pCam->Init();
  		
  	// Retrieve GenICam nodemap
  	INodeMap & nodeMap = pCam->GetNodeMap();
    CFloatPtr pNode = (CFloatPtr)nodeMap.GetNode("DeviceTemperature");
  
  	for (int i=0; i<100000; i++) {
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
