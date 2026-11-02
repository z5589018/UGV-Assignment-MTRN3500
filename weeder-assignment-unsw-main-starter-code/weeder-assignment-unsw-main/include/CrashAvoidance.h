#include <UGVModule.h>

ref class CrashAvoidance : public UGVModule {
public:
    CrashAvoidance() {};
    // Create shared memory objects
    virtual error_state setupSharedMemory();

    // Send/Recieve data from shared memory structures
    virtual error_state processSharedMemory() override;

    // Shutdown all modules in the software
    void shutdownModules();

    // Get Shutdown signal for module, from Thread Management SM
    virtual bool getShutdownFlag() override;

    // Thread function for TMM
    virtual void threadFunction() override;

};