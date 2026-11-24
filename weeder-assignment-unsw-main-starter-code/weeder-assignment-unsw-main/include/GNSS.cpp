// Create the LiDAR module below!
#include "GNSS.h"

#include <SMObjects.h>

#define CRC32_POLYNOMIAL 0xEDB88320L

GNSS::GNSS(SM_ThreadManagement^ SM_TM, SM_GNSS^ SM_gnss) {
	SM_TM_ = SM_TM;
	GNSS_SM_GNSS_ = SM_gnss;

}
// Send/Recieve data from shared memory structures
error_state GNSS::processSharedMemory() {
	if (ReadData == nullptr) {
		return SUCCESS;
	}
	pin_ptr<unsigned char> ucBuffer = &ReadData[0];
	unsigned char H = ReadData[3];

	// Making an assumption that the data packet isn't going to be much different from the one stipulated is the GPS Manual:
	// i.e. assuming that the header is 28 bytes long
	// From GPS manual, we know that:

	// Takes the ReadData[H+16] byte and converts 8 bytes from there to northing, easting, etc. 
	double northing = BitConverter::ToDouble(ReadData, H + 16);
	double easting = BitConverter::ToDouble(ReadData, H + 24);
	double height = BitConverter::ToDouble(ReadData, H + 32);

	int CRC = H + 80;

	if (CalculateBlockCRC32(CRC, ucBuffer) != BitConverter::ToInt32(ReadData, CRC)) {
		return ERR_INVALID_DATA;
	}
	Monitor::Enter(GNSS_SM_GNSS_->lockObject);
	GNSS_SM_GNSS_->Northing = northing;
	GNSS_SM_GNSS_->Easting = easting;
	GNSS_SM_GNSS_->Height = height;
	Monitor::Exit(GNSS_SM_GNSS_->lockObject);

	return SUCCESS;
}

// Get Shutdown signal for module, from Thread Management SM
bool GNSS::getShutdownFlag() {
	return (SM_TM_->shutdown & bit_GNSS) != 0;
}

// Thread function for TMM
void GNSS::threadFunction() {
	while (!getShutdownFlag()) {
		Monitor::Enter(SM_TM_->lockObject);
		SM_TM_->heartbeat |= bit_GNSS;
		Monitor::Exit(SM_TM_->lockObject);
		connect(WEEDER_ADDRESS, 24000); //Assuming the hostname is weeder (it might not be)(also GNSS uses 24000)
		communicate();
		processSharedMemory();

		Thread::Sleep(50);
	}
	Stream->Close();
	Client->Close();
}

error_state GNSS::connect(String^ hostName, int portNumber) {
	try {
		Client = gcnew TcpClient(hostName, portNumber);
		Stream = Client->GetStream();
		Client->NoDelay = true;
		Client->ReceiveTimeout = 500;
		Client->SendTimeout = 500;
		Client->ReceiveBufferSize = 1024;
		Client->SendBufferSize = 1024;
	}
	catch (System::Exception^ e) {
		return ERR_CONNECTION;
	}
	
	return SUCCESS;
	

}

error_state GNSS::communicate() {

	// Error that the ReadData is null when using simulator. Perhaps its the simulator or my bad code :( But the GPS is connected to the simulator so idk
	if (ReadData != nullptr) {

		try {
			int bit = Stream->Read(ReadData, 0, ReadData->Length);
			if (bit < 4) {
				return ERR_NO_DATA;
			}
		}
		catch (Exception^ e) {
			Console::WriteLine("Error with reading GNSS data: ", e);
			return ERR_CONNECTION;
		}
	}
	

	return SUCCESS;
}

// Code from Appendix A: 
unsigned long GNSS::CRC32Value(int i) {
	int j;
	unsigned long ulCRC;
	ulCRC = i;
	for (j = 8; j > 0; j--)
	{
		if (ulCRC & 1)
			ulCRC = (ulCRC >> 1) ^ CRC32_POLYNOMIAL;
		else
			ulCRC >>= 1;
	}
	return ulCRC;
}

unsigned long GNSS::CalculateBlockCRC32(unsigned long ulCount, unsigned char* ucBuffer) {
	unsigned long ulTemp1;
	unsigned long ulTemp2;
	unsigned long ulCRC = 0;

	while (ulCount-- != 0) {
		ulTemp1 = (ulCRC >> 8) & 0x00FFFFFFL;
		ulTemp2 = CRC32Value(((int)ulCRC ^ * ucBuffer++) & 0xff);
		ulCRC = ulTemp1 ^ ulTemp2;
	}
	return ulCRC;
}

