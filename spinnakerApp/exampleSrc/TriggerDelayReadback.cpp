 
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

    CEnumerationPtr pTriggerModeNode         = nodeMap.GetNode("TriggerMode");
    CEnumerationPtr pTriggerSourceNode       = nodeMap.GetNode("TriggerSource");
    CEnumerationPtr pTriggerActivationNode   = nodeMap.GetNode("TriggerActivation");    
    CEnumerationPtr pTriggerOverlapNode      = nodeMap.GetNode("TriggerOverlap");
    CBooleanPtr     pTriggerDelayEnabledNode = nodeMap.GetNode("TriggerDelayEnabled");
    CFloatPtr       pTriggerDelayNode        = nodeMap.GetNode("TriggerDelay");
    CEnumEntryPtr entry;

    entry = pTriggerModeNode->GetEntryByName("On");
    cout << "Setting TriggerMode: " << entry->GetValue() << endl;    
    pTriggerModeNode->SetIntValue(entry->GetValue());

    entry = pTriggerSourceNode->GetEntryByName("Line0");
    cout << "Setting TriggerSource: " << entry->GetValue() << endl;    
    pTriggerSourceNode->SetIntValue(entry->GetValue());

    entry = pTriggerActivationNode->GetEntryByName("FallingEdge");
    cout << "Setting TriggerActivation: " << entry->GetValue() << endl;    
    pTriggerActivationNode->SetIntValue(entry->GetValue());

    entry = pTriggerOverlapNode->GetEntryByName("Off");
    cout << "Setting TriggerOverlap: " << entry->GetValue() << endl;    
    pTriggerOverlapNode->SetIntValue(entry->GetValue());
    
    cout << "Setting TriggerDelayEnabled: true " << endl;    
    *pTriggerDelayEnabledNode = true;

    double triggerDelayValue = 10000.; 
    cout << "TriggerDelay min: " << pTriggerDelayNode->GetMin() << " max: " << pTriggerDelayNode->GetMax() << endl;
    cout << "Setting TriggerDelay: " << triggerDelayValue << endl;    
    pTriggerDelayNode->SetValue(triggerDelayValue);
    cout << "TriggerValue readback value: " << pTriggerDelayNode->GetValue() << endl;
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
