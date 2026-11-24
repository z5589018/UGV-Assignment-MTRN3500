#include "TMM.h"
#include "LiDAR.h"
#include "Display.h"
#include "GNSS.h"
#include "VC.h"
#include "SMObjects.h"
#include <vector>
#include <CrashAvoidance.h>

error_state ThreadManagement::setupSharedMemory() {
	SM = gcnew SM_ThreadManagement();
	gnssSM = gcnew SM_GNSS();
	sm_vc = gcnew SM_VehicleControl();
	lidarSM = gcnew SM_LiDAR();

	if (SM == nullptr) {
		return ERR_NO_DATA;
	}
	SM->heartbeat = 0;
	SM->shutdown = 0;

	// Creating 4 different stopwatches for each module
	SM->WatchList = gcnew array<Stopwatch^>(4);
	for (int i = 0; i < 4; i++) {
		SM->WatchList[i] = gcnew Stopwatch();
	}
	
	// NOTE: Might need to include other error returns below
	return error_state::SUCCESS;
}

error_state ThreadManagement::processSharedMemory() {
	// This is the heartbeak check. When this is called, we need to see if each module has checked in or not. If not, we sound the alarms
	Monitor::Enter(SM->lockObject);
	// Vector of bit corresponding to each module
	std::vector<uint8_t> bits = { bit_LiDAR, bit_GNSS, bit_VC, bit_DISPLAY };
	
	for (int i = 0; i < 4; i++) {
		// If the heartbeat and the bit are the same, then the bit gets reset to 0 for the module to change back to 1 (if it works correctly)
		if (SM->heartbeat & bits[i]) {
			SM->heartbeat &= ~bits[i]; // Resetting the individual bit to 0
			SM->WatchList[i]->Restart();
		}
		// Else if elapsed time > 100 milliseconds or whatever we decided on
		else if(SM->WatchList[i]->ElapsedMilliseconds > 100) {
			shutdownModules();
			return ERR_CONNECTION;
		}
	}
	Monitor::Exit(SM->lockObject);
	return error_state::SUCCESS;
}

void ThreadManagement::shutdownModules() {
	// Need to review the order at which the modules are shutting down!!
	std::vector<uint8_t> bits = { bit_LiDAR, bit_GNSS, bit_VC, bit_DISPLAY };

	Monitor::Enter(SM->lockObject);
	for (int i = 4; i > 0; i--) {
		SM->shutdown = bits[i];
	}
	Monitor::Exit(SM->lockObject);
}

bool ThreadManagement::getShutdownFlag() {
	// Only checking the TM bit and the shutdown bit
	return (SM->shutdown & bit_TM) != 0;
}

void ThreadManagement::threadFunction() {
	int Heartbeat = 200; // Heartbeat milliseconds needs to be changed to fit situation better

	// Ensuring that the Shared Memory is setup correctly
	if (setupSharedMemory() != SUCCESS) {
		return;
	}

	// Creating each module within NetworkedModule

	LiDAR^ lidar = gcnew LiDAR(SM, lidarSM);
	Display^ display = gcnew Display(SM, lidarSM, gnssSM);
	GNSS^ gnss = gcnew GNSS(SM, gnssSM);
	VC^ vehicle= gcnew VC(SM, sm_vc);

	ThreadList = gcnew array<Thread^>(4); // Each index holds a thread for each module
	
	// Setting up the thread of each module

	
	// Need to review the order of starting the threads!!
	ThreadList[0] = gcnew Thread(gcnew ThreadStart(lidar, &LiDAR::threadFunction));
	ThreadList[1] = gcnew Thread(gcnew ThreadStart(gnss, &GNSS::threadFunction));
	ThreadList[2] = gcnew Thread(gcnew ThreadStart(vehicle, &VC::threadFunction));
	ThreadList[3] = gcnew Thread(gcnew ThreadStart(display, &Display::threadFunction));

	// Starting all the threads:
	/*
	for (int i = 0; i < 4; i++) {
		ThreadList[i]->Start();
	}
	*/
	ThreadList[2]->Start();
	// Heartbeat and shutdownflag
	while (!getShutdownFlag()) {
		processSharedMemory();
		if (System::Console::KeyAvailable && System::Console::ReadKey(true).KeyChar == 'Q') { // If key q is pressed, shutdown of all modules is forced
			shutdownModules();
		}
		Thread::Sleep(Heartbeat); //Checks for shutdown flags every Heartbeat seconds
	}

	// Shutting down but waiting for each module to go through its own shutdown sequence
	for (int i = 0; i < 4; i++) {
		ThreadList[i]->Join();
		std::cout << "Module: " << i << "successfully shutdown" << std::endl; // Used for debugging needs to be deleted in future
	}


}
