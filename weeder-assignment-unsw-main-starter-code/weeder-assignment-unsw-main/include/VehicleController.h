#pragma once
#include <UGVModule.h>
#include <NetworkedModule.h>

ref class VehicleController : public NetworkedModule {
public:

	VehicleController() {}; //Default constructor

	VehicleController(SM_ThreadManagement^ SM_TM, SM_VehicleControl^ VEHCTRL_SM_);
	// Send/Recieve data from shared memory structures
	error_state processSharedMemory() override;

	// Shutdown all modules in the software
	void shutdownModules();

	// Get Shutdown signal for module, from Thread Management SM
	bool getShutdownFlag() override;

	// Thread function for TMM
	void threadFunction() override;

	virtual error_state connect(String^ hostName, int portNumber) override;	// Establish TCP connection

	virtual error_state communicate() override;								// Communicate over TCP connection (includes any error checking required on data)


private:

	// List of inherited data members:
	//SM_ThreadManagement^ SM_TM_;
	//TcpClient^ Client;					// Handle for TCP connection
	//NetworkStream^ Stream;				// Handle for TCP data stream
	//array<unsigned char>^ ReadData;		// Array to store sensor Data (only used for sensor modules)

	SM_VehicleControl^ VEHCTRL_SM_;

};
