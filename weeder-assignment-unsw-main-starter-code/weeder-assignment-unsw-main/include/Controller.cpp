#include "Controller.h"
#include <SMObjects.h>



Controller::Controller() {};
Controller::Controller(double leftThumbX, double leftThumbY, double rightThumbX, double rightThumbY) {
	
	ControllerInterface controller;
	/*
	Monitor::Enter(VEHCTRL_SM_->lockObject);
	VEHCTRL_SM_->Speed = 10;
	VEHCTRL_SM_->Steering = 10;
	Monitor::Exit(VEHCTRL_SM_->lockObject);
	*/
	
	auto state = controller.GetState();
	state.leftThumbX = leftThumbX;
	state.leftThumbY = leftThumbY;
	state.rightThumbX = rightThumbX;
	state.rightThumbY = rightThumbY;
	
};
/**
	 * Send/Recieve data from shared memory structures
	 *
	 * @returns the success of this function at completion
	*/
error_state Controller::processSharedMemory() {

	try {
		ControllerInterface(1, 1); // Input type 0 for x-box controller and 1 for keyboard. Using keyboard since testing

		int speedMultiplier = 1; //How much faster the vehicle moves, changed through trial and error;
		int steeringMultiplier = 40; //Converting -1 to 1 to degrees, received from assignment spec 

		// Take the value from the SM and change steering and speed accordingly
		ControllerInterface controller;
		Console::WriteLine(controller.IsConnected());
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

		Monitor::Exit(VEHCTRL_SM_->lockObject);
		return SUCCESS;
	}

	catch (Exception^ e) {
		Console::WriteLine("Error with VC: ", e);
		return ERR_CONNECTION;
	}
	
	return SUCCESS;
};

/**
 * Get Shutdown signal for the current, from Process Management SM
 *
 * @returns the current state of the shutdown flag
*/
bool Controller::getShutdownFlag() {
	return (SM_TM_->shutdown & bit_CONTROLLER) != 0;
};

/**
 * Main runner for the thread to perform all required actions
*/
void Controller::threadFunction() {
	int Heartbeat = 50; // Heartbeat milliseconds needs to be changed to fit situation better
	while (!getShutdownFlag()) {
		processSharedMemory();
		Thread::Sleep(Heartbeat);
	}
	
};

/**
 * A helper function for printing a helpful error message based on the error type
 * returned from a function
 *
 * @param error the error that should be printed out
*/
void Controller::printError(error_state error)
{
	switch (error)
	{
	case SUCCESS:
		std::cout << "Success." << std::endl;
		break;
	case ERR_NO_DATA:
		std::cout << "ERROR: No Data Available." << std::endl;
		break;
	case ERR_INVALID_DATA:
		std::cout << "ERROR: Invalid Data Received." << std::endl;
		break;
		// ADD PRINTOUTS FOR OTHER ERROR TYPES
	}
}
