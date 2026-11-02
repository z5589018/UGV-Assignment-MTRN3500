#include "VehicleController.h"
#include <UGVModule.h>
#include <NetworkedModule.h>
#include <SMObjects.h>

VehicleController::VehicleController(SM_ThreadManagement^ SM_TM, SM_VehicleControl^ VEHCTRL_SM_) {

}
// Send/Recieve data from shared memory structures
error_state VehicleController::processSharedMemory() {

}

// Shutdown all modules in the software
void VehicleController::shutdownModules() {

}

// Get Shutdown signal for module, from Thread Management SM
bool VehicleController::getShutdownFlag() {

}

// Thread function for TMM
void VehicleController::threadFunction() {

}

error_state VehicleController::connect(String^ hostName, int portNumber) {

}	// Establish TCP connection

error_state VehicleController::communicate() {

}