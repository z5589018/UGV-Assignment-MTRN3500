#include "Controller.h"
#include <SMObjects.h>

Controller::Controller() {};
Controller::Controller(double leftThumbX, double leftThumbY, double rightThumbX, double rightThumbY) {

};
/**
	 * Send/Recieve data from shared memory structures
	 *
	 * @returns the success of this function at completion
	*/
error_state Controller::processSharedMemory() {

};

/**
 * Get Shutdown signal for the current, from Process Management SM
 *
 * @returns the current state of the shutdown flag
*/
bool Controller::getShutdownFlag() {

};

/**
 * Main runner for the thread to perform all required actions
*/
void Controller::threadFunction() {

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
