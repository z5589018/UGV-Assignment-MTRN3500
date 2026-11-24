#include "Display.h"

Display::Display() {}

Display::Display(SM_ThreadManagement^ SM_TM, SM_LiDAR^ SM_lidar, SM_GNSS^ SM_gnss) {
	SM_TM_ = SM_TM;
	Display_SM_LiDAR_ = SM_lidar;
	Display_SM_GNSS_ = SM_gnss;
}
// Send/Recieve data from shared memory structures
error_state Display::processSharedMemory() {
	if (Display_SM_LiDAR_ == nullptr) {
		return ERR_NO_DATA;
	}
	// From FakeLiDAR. Maybe needs to be changed??
	for (int i = 0; i < 361; i++) {
		Display_SM_LiDAR_->x[i] = 5000 * System::Math::Sin(i * 0.5 * System::Math::PI / 180.0);
		Display_SM_LiDAR_->y[i] = -5000 * System::Math::Cos(i * 0.5 * System::Math::PI / 180.0);
	}
	return SUCCESS;
}

// Get Shutdown signal for module, from Thread Management SM
bool Display::getShutdownFlag() {
	return true;
}

// Thread function for TMM
void Display::threadFunction() {
	while (!getShutdownFlag()) {
		Monitor::Enter(SM_TM_->lockObject);
		SM_TM_->heartbeat =  bit_DISPLAY;
		Monitor::Exit(SM_TM_->lockObject);
		connect(WEEDER_ADDRESS, 28000); //Assuming the hostname is weeder (it might not be)
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

error_state Display::connect(System::String^ hostName, int portNumber) {
	Client = gcnew TcpClient(hostName, portNumber);
	Stream = Client->GetStream();
	Client->NoDelay = true;
	Client->ReceiveTimeout = 500;
	Client->SendTimeout = 500;
	Client->ReceiveBufferSize = 1024;
	Client->SendBufferSize = 1024;

	SendData = gcnew array<unsigned char>(64);
	ReadData = gcnew array<unsigned char>(64);

	return SUCCESS;

}

error_state Display::communicate() {
	sendDisplayData();
	return SUCCESS;
}

void Display::FakeLiDAR() {
	Display_SM_LiDAR_ = gcnew SM_LiDAR;

	for (int i = 0; i < 361; i++) {
		Display_SM_LiDAR_->x[i] = 5000 * System::Math::Sin(i * 0.5 * System::Math::PI / 180.0);
		Display_SM_LiDAR_->y[i] = -5000 * System::Math::Cos(i * 0.5 * System::Math::PI / 180.0);
	}
}

void Display::sendDisplayData()
{
	// Serialize the data arrays to a byte array
	//(format required for sending)

	Monitor::Enter(Display_SM_LiDAR_->lockObject);

	array<System::Byte>^ dataX = gcnew array<System::Byte>(Display_SM_LiDAR_->x->Length * sizeof(double));
	System::Buffer::BlockCopy(Display_SM_LiDAR_->x, 0, dataX, 0, dataX->Length);
	array<System::Byte>^ dataY = gcnew array<System::Byte>(Display_SM_LiDAR_->y->Length * sizeof(double));
	System::Buffer::BlockCopy(Display_SM_LiDAR_->y, 0, dataY, 0, dataY->Length);
	Monitor::Exit(Display_SM_LiDAR_->lockObject);

	// Send byte data over connection
	Stream->Write(dataX, 0, dataX->Length);
	Thread::Sleep(10);
	Stream->Write(dataY, 0, dataY->Length);
}