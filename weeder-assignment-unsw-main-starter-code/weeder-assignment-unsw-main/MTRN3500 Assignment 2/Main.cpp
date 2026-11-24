#include <Controller.h>
#include "Display.h"
#include <conio.h>
#include <LiDAR.h>
#include <GNSS.h>
#include <TMM.h>
#include <VC.h>

int main(void) {
    SM_ThreadManagement^ smthread = gcnew SM_ThreadManagement();
    SM_VehicleControl^ sm_vc = gcnew SM_VehicleControl();

    ThreadManagement^ thread = gcnew ThreadManagement();
    Controller^ ctrl = gcnew Controller(0,0,0,0);
    VC^ vc = gcnew VC(smthread, sm_vc);
    
    thread->threadFunction();
    ctrl->threadFunction();
    vc->threadFunction();

    return 0;
}
