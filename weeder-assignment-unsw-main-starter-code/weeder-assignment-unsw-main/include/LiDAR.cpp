// Create the LiDAR module below!
#include "LiDAR.h"

#include <SMObjects.h>

LiDAR::LiDAR(SM_ThreadManagement^ SM_TM, SM_LiDAR^ SM_lidar) {
	SM_TM_ = SM_TM;
	LiDAR_SM_LiDAR_ = SM_lidar;

}
// Send/Recieve data from shared memory structures
error_state LiDAR::processSharedMemory() {
	
	Stream->Read(ReadData, 0, ReadData->Length);
	System::String^ Response = Encoding::ASCII->GetString(ReadData);

	array<System::String^>^ Fragments = Response->Split(' ');
	int numPoints = System::Convert::ToInt32(Fragments[25], 16);


	Monitor::Enter(LiDAR_SM_LiDAR_->lockObject);

	for (int i = 0; i < 361; i++) {
		LiDAR_SM_LiDAR_->x[i] = System::Convert::ToInt32(Fragments[116 + i], 16) * System::Math::Sin(i * 0.5 * System::Math::PI / 180.0);
		LiDAR_SM_LiDAR_->y[i] = -System::Convert::ToInt32(Fragments[116 + i], 16) * System::Math::Cos(i * 0.5 * System::Math::PI / 180.0);
	}

	Monitor::Exit(LiDAR_SM_LiDAR_->lockObject);

	if (numPoints == 0x21D) {
		return SUCCESS;
	}
	return ERR_INVALID_DATA;
	
	return SUCCESS;
}

// Get Shutdown signal for module, from Thread Management SM
bool LiDAR::getShutdownFlag() {
	return (SM_TM_->shutdown & bit_LiDAR) != 0;
}

// Thread function for TMM
void LiDAR::threadFunction() {
	// Outline the process that the LiDAR needs to take??
		connect(WEEDER_ADDRESS, 23000);
	while (!getShutdownFlag()) {
		Monitor::Enter(SM_TM_->lockObject);
		SM_TM_->heartbeat |= bit_LiDAR; 
		Monitor::Exit(SM_TM_->lockObject);
		communicate();
		processSharedMemory();

		Thread::Sleep(50);
	}
	if (Stream != nullptr) {
		Stream->Close();
	}
	if (Client != nullptr) {
		Client->Close();
	}
}

error_state LiDAR::connect(System::String^ hostName, int portNumber) {

	//Step 1: Authorise to change the configuration
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

	/*
	//Step2: Configure scan frequency, angle and resolution
	try {
		System::String^ scanCfg = "sMN mLMPsetscancfg +5000 +1 +5000 –450000 +2250000";
		SendData = Encoding::ASCII->GetBytes(scanCfg);
		Stream->WriteByte(0x02);
		Stream->Write(SendData, 0, SendData->Length);
		Stream->WriteByte(0x03);

		Console::WriteLine("Sent command 1, awaiting response!");

		int len = Stream->Read(ReadData, 0, ReadData->Length);
		String^ resp = Encoding::ASCII->GetString(ReadData, 0, len);
		System::Console::WriteLine("Scan config response: \n{0}", resp);
		
		if (int len = Stream->Read(ReadData, 0, ReadData->Length) != 0) {
			String^ resp = Encoding::ASCII->GetString(ReadData, 0, len);
			System::Console::WriteLine("Scan config response: \n{0}", resp);
		}
		else {
			Console::WriteLine("No Response");
		}
		
	}
	catch (Exception^ e2) {
		Console::WriteLine("Failed Stage 1:", e2->Message);
		return ERR_CONNECTION;
	}
	try {
		String^ scanCfg = "sWN LMDscandatacfg 01 00 0 0 0 00 00 0 0 0 0 +1";
		SendData = Encoding::ASCII->GetBytes(scanCfg);
		Stream->WriteByte(0x02);
		Stream->Write(SendData, 0, SendData->Length);
		Stream->WriteByte(0x03);
		System::Threading::Thread::Sleep(100);
		Console::WriteLine("Sent command 2, awaiting response!");

		int len = Stream->Read(ReadData, 0, ReadData->Length);
		String^ resp = Encoding::ASCII->GetString(ReadData, 0, len);
		System::Console::WriteLine("Scan config response: \n{0}", resp);
		
		if (int len = Stream->Read(ReadData, 0, ReadData->Length) != 0) {
			String^ resp = Encoding::ASCII->GetString(ReadData, 0, len);
			System::Console::WriteLine("Scan config response: \n{0}", resp);
		}
		else {
			Console::WriteLine("No Response");
		}
		
	}
	catch (Exception^ e2) {
		Console::WriteLine("Failed Configuration: ", e2->Message);
		return ERR_CONNECTION;
	}
	try {
		String^ scanCfg = "sWN LMPoutputRange 1 1388 0 +1800000";
		SendData = Encoding::ASCII->GetBytes(scanCfg);
		Stream->WriteByte(0x02);
		Stream->Write(SendData, 0, SendData->Length);
		Stream->WriteByte(0x03);
		System::Threading::Thread::Sleep(100);
		Console::WriteLine("Sent command 3, awaiting response!");
	}
	catch (Exception^ e2) {
		Console::WriteLine("Failed Stage 3: ", e2->Message);
		return ERR_CONNECTION;
	}
	//Step 3: Configure LiDAR to output only DIST1 channel
	try {
		System::String^ dataCfg = "sWN LMDscandatacfg 01 00 0 0 0 00 00 0 0 0 0 +1";
		SendData = Encoding::ASCII->GetBytes(dataCfg);
		Stream->WriteByte(0x02);
		Stream->Write(SendData, 0, SendData->Length);
		Stream->WriteByte(0x03);
		Thread::Sleep(100);
		
		
		if (int len = Stream->Read(ReadData, 0, ReadData->Length) != 0) {
			String^ resp = Encoding::ASCII->GetString(ReadData, 0, len);
			System::Console::WriteLine("Channel config response: \n{0}", resp);
		}
		else {
			Console::WriteLine("No Response");
		}
		
	}
	catch(Exception^ e3) {
		Console::WriteLine(" Failed Configuration LiDAR: ", e3->Message);
		return ERR_CONNECTION;
	}
	*/

	return SUCCESS;

}

error_state LiDAR::communicate() {
	//sRN LMDscandata
	/*
	System::String^ Command = "sRN LMDscandata";
	SendData = Encoding::ASCII->GetBytes(Command);
	Stream->WriteByte(0x02);
	Stream->Write(SendData, 0, SendData->Length);
	Stream->WriteByte(0x03);
	Thread::Sleep(100);
	*/
	if (ReadData != nullptr) {
		int len = Stream->Read(ReadData, 0, ReadData->Length);
	
		System::String^ Response= Encoding::ASCII->GetString(ReadData, 0, len);
		System::Console::WriteLine(Response);
		System::Console::WriteLine();
	}
	
	return SUCCESS;
}

