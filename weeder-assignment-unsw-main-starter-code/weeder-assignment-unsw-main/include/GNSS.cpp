// Create the LiDAR module below!
#include "GNSS.h"

#include <SMObjects.h>

GNSS::GNSS(SM_ThreadManagement^ SM_TM, SM_GNSS^ SM_gnss) {
	SM_TM_ = SM_TM;
	GNSS_SM_GNSS_ = SM_gnss;

}
// Send/Recieve data from shared memory structures
error_state GNSS::processSharedMemory() {
	/*
	Stream->Read(ReadData, 0, ReadData->Length);
	String^ Response = Encoding::ASCII->GetString(ReadData);

	array<String^>^ Fragments = Response->Split(' ');
	int numPoints = Convert::ToInt32(Fragments[25], 16);


	Monitor::Enter(GNSS_SM_LiDAR_->lockObject);

	for (int i = 0; i < 361; i++) {
		GNSS_SM_GNSS_->x[i] = Convert::ToInt32(Fragments[116 + i], 16) * Math::Sin(i * 0.5 * Math::PI / 180.0);
		GNSS_SM_GNSS_->y[i] = -Convert::ToInt32(Fragments[116 + i], 16) * Math::Cos(i * 0.5 * Math::PI / 180.0);
	}

	Monitor::Exit(LiDAR_SM_LiDAR_->lockObject);

	if (numPoints == 0x21D) {
		return SUCCESS;
	}
	return ERR_INVALID_DATA;
	*/
	return SUCCESS;
}

// Shutdown all modules in the software
void GNSS::shutdownModules() {

}

// Get Shutdown signal for module, from Thread Management SM
bool GNSS::getShutdownFlag() {
	return true;
}

// Thread function for TMM
void GNSS::threadFunction() {

}

error_state GNSS::connect(String^ hostName, int portNumber) {
	/*
	Client = gcnew TcpClient(hostName, portNumber);
	Stream = Client->GetStream();
	Client->NoDelay = true;
	Client->ReceiveTimeout = 500;
	Client->SendTimeout = 500;
	Client->ReceiveBufferSize = 1024;
	Client->SendBufferSize = 1024;

	SendData = gcnew array<unsigned char>(512);
	ReadData = gcnew array<unsigned char>(8192);

	//Step 1: Authorise to change the configuration
	String^ authorCmd = "sMn SetAccessMore 03 F4724744";
	SendData = Encoding::ASCII->GetBytes(authorCmd);
	Stream->WriteByte(0x02);
	Stream->Write(SendData, 0, SendData->Length);
	Stream->WriteByte(0x03);
	System::Threading::Thread::Sleep(100);

	int len = Stream->Read(ReadData, 0, ReadData->Length);
	String^ resp = Encoding::ASCII->GetString(ReadData, 0, len);
	Console::WriteLine("Auth response: \n{0}", resp);

	//Step2: Configure scan frequency, angle and resolution
	String^ scanCfg = "sMn mLMPsetscanfg +5000 + +5000 +0 +1800000";

	SendData = Encoding::ASCII->GetBytes(scanCfg);
	Stream->WriteByte(0x02);
	Stream->Write(SendData, 0, SendData->Length);
	Stream->WriteByte(0x03);
	Thread::Sleep(100);

	len = Stream->Read(ReadData, 0, ReadData->Length);
	resp = Encoding::ASCII->GetString(ReadData, 0, len);
	Console::WriteLine("Scan config response: \n{0}", resp);

	//Step 3: Configure LiDAR to output only DIST1 channel
	String^ dataCfg = "sWn LMDscandatacfg 01 00 0 0 0 00 00 0 0 0 0 +1";
	SendData = Encoding::ASCII->GetBytes(dataCfg);
	Stream->WriteByte(0x02);
	Stream->Write(SendData, 0, SendData->Length);
	Stream->WriteByte(0x03);
	Thread::Sleep(100);

	len = Stream->Read(ReadData, 0, ReadData->Length);
	resp = Encoding::ASCII->GetString(ReadData, 0, len);
	Console::WriteLine("Channel config response: \n{0}", resp);

	*/
	return SUCCESS;
	

}

error_state GNSS::communicate() {
	/*
	//sRN LMDscandata
	String^ Command = "sRN LMDscandata";
	SendData = Encoding::ASCII->GetBytes(Command);
	Stream->WriteByte(0x02);
	Stream->Write(SendData, 0, SendData->Length);
	Stream->WriteByte(0x03);
	Thread::Sleep(100);

	int len = Stream->Read(ReadData, 0, ReadData->Length);
	String^ Response = Encoding::ASCII->GetString(ReadData, 0, len);
	Console::WriteLine(Response);
	Console::WriteLine();
	*/
	return SUCCESS;
}

