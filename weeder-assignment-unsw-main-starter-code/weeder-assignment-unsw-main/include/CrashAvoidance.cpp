#include <CrashAvoidance.h>

// Send/Recieve data from shared memory structures
CrashAvoidance::CrashAvoidance(SM_ThreadManagement^ SM, SM_LiDAR^ SMLiDAR, SM_VehicleControl^ SMVC) {
	sm = SM;
	sm_lidar = SMLiDAR;
	sm_vc = SMVC;
}
error_state CrashAvoidance::processSharedMemory() {
	bool obstacle = false;
	double x;
	double y;

	Monitor::Enter(sm_lidar->lockObject);
	// Checking every data point
	for (int i = 0; i < 361; i++) {
		x = sm_lidar->x[i];
		y = sm_lidar->y[i];

		// Checking if there's an obstacle nearby
		if (x > 50 && x < 1500 && Math::Abs(y) < 350) {
			obstacle = true;
			break;
		}
	}
	Monitor::Exit(sm_lidar->lockObject);
	if (obstacle) {
		Monitor::Enter(sm_vc->lockObject);
		if (sm_vc->Speed > 0) {
			double theta;
			sm_vc->Speed = 0;
			sm_vc->Steering = 0; // Car is facing straight now

			theta = Math::Atan(x / Math::Abs(y)) * (180.0 / Math::PI);
			sm_vc->Steering = (90 - theta); // Car's wheels should be facing the obstacle
			sm_vc->Speed = -1; //Going backwards
			Thread::Sleep(500); // Travelling at that speed for 0.5s
			sm_vc->Steering = -40;
			sm_vc->Speed = 1; // Travelling all the way to the left
			Thread::Sleep(500); // For 0.5s
			sm_vc->Steering = 0; // Going forwards
		}
		Monitor::Exit(sm_vc->lockObject);
		obstacle = false;
	}
	return SUCCESS;
}

// Get Shutdown signal for module, from Thread Management SM
bool CrashAvoidance::getShutdownFlag() {
	return (sm->heartbeat & bit_CRASHAVOIDANCE);
}

// Thread function for TMM
void CrashAvoidance::threadFunction() {
	while (!getShutdownFlag()) {
		processSharedMemory();
		Thread::Sleep(50);
	}
}