#pragma once
#include <UGVModule.h>
#include <ControllerInterface.h>

ref class Controller : public UGVModule {
public:

	Controller() {};
	Controller(double leftThumbX, double leftThumbY, double rightThumbX, double rightThumbY) {

	};
	/**
		 * Send/Recieve data from shared memory structures
		 *
		 * @returns the success of this function at completion
		*/
	virtual error_state processSharedMemory() override;

	/**
	 * Get Shutdown signal for the current, from Process Management SM
	 *
	 * @returns the current state of the shutdown flag
	*/
	virtual bool getShutdownFlag() override;

	/**
	 * Main runner for the thread to perform all required actions
	*/
	virtual void threadFunction() override;

	/**
	 * A helper function for printing a helpful error message based on the error type
	 * returned from a function
	 *
	 * @param error the error that should be printed out
	*/
	static void printError(error_state error)
	{
		
	}



private:

	// List of inherited data members:
	//SM_ThreadManagement^ SM_TM_;
	//TcpClient^ Client;					// Handle for TCP connection
	//NetworkStream^ Stream;				// Handle for TCP data stream
	//array<unsigned char>^ ReadData;		// Array to store sensor Data (only used for sensor modules)

};


#pragma once
#include <UGVModule.h>