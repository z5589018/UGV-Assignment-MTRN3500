#pragma once
#include <UGVModule.h>


ref class CrashAvoidance : public UGVModule {
public:
    CrashAvoidance() {};
    CrashAvoidance(SM_ThreadManagement^ SM, SM_LiDAR^ SMLiDAR, SM_VehicleControl^ SMVC);

    // Send/Recieve data from shared memory structures
    virtual error_state processSharedMemory() override;

    // Get Shutdown signal for module, from Thread Management SM
    virtual bool getShutdownFlag() override;

    // Thread function for TMM
    virtual void threadFunction() override;

private:
    SM_ThreadManagement^ sm;
    SM_LiDAR^ sm_lidar;
    SM_VehicleControl^ sm_vc;
};