#include "CrashAvoidance.h"
#include <UGVModule.h>

CrashAvoidance::CrashAvoidance() {};
// Create shared memory objects
error_state CrashAvoidance::setupSharedMemory() {
	return SUCCESS;
}

// Send/Recieve data from shared memory structures
error_state CrashAvoidance::processSharedMemory() {
	return SUCCESS;
}

// Shutdown all modules in the software
void CrashAvoidance::shutdownModules() {

}

// Get Shutdown signal for module, from Thread Management SM
bool CrashAvoidance::getShutdownFlag() {
	return true;
}

// Thread function for TMM
void CrashAvoidance::threadFunction() {

}