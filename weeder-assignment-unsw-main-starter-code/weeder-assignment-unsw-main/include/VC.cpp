#include <ControllerInterface.h>
#include "VC.h"
#include <UGVModule.h>
#include <NetworkedModule.h>
#include <SMObjects.h>
VC::VC() {}
VC::VC(SM_ThreadManagement^ SM_TM, SM_VehicleControl^ SM_VC) {
	SM_TM_ = SM_TM;
	VEHCTRL_SM_ = SM_VC;
	
}
// Send/Recieve data from shared memory structures
error_state VC::processSharedMemory() {
	ControllerInterface(1, 1); // Input type 0 for x-box controller and 1 for keyboard. Using keyboard since testing

	int speedMultiplier = 1; //How much faster the vehicle moves, changed through trial and error;
	int steeringMultiplier = 40; //Converting -1 to 1 to degrees, received from assignment spec 

	// Take the value from the SM and change steering and speed accordingly
	ControllerInterface controller;
	//Console::WriteLine(controller.IsConnected());
	
	if (!controller.IsConnected()) {
		Monitor::Enter(VEHCTRL_SM_->lockObject);
		VEHCTRL_SM_->Speed = 0;
		VEHCTRL_SM_->Steering = 0;
		Monitor::Exit(VEHCTRL_SM_->lockObject);
		return ERR_CONNECTION;
	}
	
	auto state = controller.GetState();
	// Calculate the right speed based on the trigger:
	Monitor::Enter(VEHCTRL_SM_->lockObject);
	// Steering:
	VEHCTRL_SM_->Steering = state.leftThumbX * steeringMultiplier;

	// Going backwards
	VEHCTRL_SM_->Speed = -speedMultiplier * state.leftTrigger;
	// Going forwards
	VEHCTRL_SM_->Speed = speedMultiplier * state.rightTrigger;


	// Keyboard mode
	VEHCTRL_SM_->Steering = state.rightThumbX;
	VEHCTRL_SM_->Speed = speedMultiplier * state.rightThumbY;
	Monitor::Enter(VEHCTRL_SM_->lockObject);
	
	
	//Test to see if VC works:

	return SUCCESS;
}

// Get Shutdown signal for module, from Thread Management SM
bool VC::getShutdownFlag() {
	return(SM_TM_->shutdown & bit_VC);
}

// Thread function for TMM
void VC::threadFunction() {
	connect(WEEDER_ADDRESS, 25000); //Assuming the hostname is weeder (it might not be)(also VC uses 25000)
	while (!getShutdownFlag()) {
		
		Monitor::Enter(SM_TM_->lockObject);
		SM_TM_->heartbeat |= bit_VC;
		Monitor::Exit(SM_TM_->lockObject);
		
		processSharedMemory();
		communicate();

		Thread::Sleep(50);
	}
	Stream->Close();
	Client->Close();
}

error_state VC::connect(System::String^ hostName, int portNumber) {
	// Authorise the connection by logging in:

	Client = gcnew TcpClient(hostName, portNumber);
	Client->NoDelay = true;
	Client->ReceiveTimeout = 500;
	Client->SendTimeout = 500;
	Client->ReceiveBufferSize = 1024;
	Client->SendBufferSize = 1024;

	Stream = Client->GetStream();

	SendData = gcnew array<unsigned char>(512);
	ReadData = gcnew array<unsigned char>(8192);

	try {

		System::String^ authorCmd = "5589018\n";
		SendData = Encoding::ASCII->GetBytes(authorCmd);

		Stream->Write(SendData, 0, SendData->Length);
		//Stream->WriteByte(0x03);
		System::Threading::Thread::Sleep(100);
		int len = Stream->Read(ReadData, 0, ReadData->Length);
		String^ resp = Encoding::ASCII->GetString(ReadData, 0, len);
		System::Console::WriteLine("Auth response: \n{0}", resp);
		if (!resp->Contains("OK")) {
			System::Console::WriteLine("Connection not Authorised");
			return ERR_CONNECTION;
		}
	}
	catch (Exception^ e1) {
		Console::WriteLine("Failed authorisation: ", e1->Message);
		return ERR_CONNECTION;
	}



	return SUCCESS;

}	// Establish TCP connection

error_state VC::communicate() {
	try {
		// Needs to communicate to the Simulator / Actual vehicle based on the SM!!
		// Sending commands to the vehicle/simulator on port 25000
		static int wdog = 0;
		wdog = 1 - wdog;

		String^ VCcmd = String::Format("# {0:F2} {1:F2} {2} #", VEHCTRL_SM_->Steering, VEHCTRL_SM_->Speed, wdog);
		array<unsigned char>^ send = Encoding::ASCII->GetBytes(VCcmd);
		Stream->Write(send, 0, send->Length);

	}
	catch (Exception^) {
		return ERR_CONNECTION;
	}
}