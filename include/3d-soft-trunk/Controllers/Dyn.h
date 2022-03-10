#pragma once

#include "3d-soft-trunk/ControllerPCC.h"

class Dyn: public ControllerPCC
{
public:
    Dyn(const SoftTrunkParameters st_params);

private:
    void control_loop();
    
    VectorXd Kp;
    VectorXd Kd;

};