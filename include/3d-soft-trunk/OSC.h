#pragma once

#include "ControllerPCC.h"

class OSC: public ControllerPCC
{
public:

    OSC(CurvatureCalculator::SensorType sensor_type);

    

private:
    
    void control_loop();

    /** @brief gains for OSC*/
    double kp;
    double kd;

    /** @brief operational dynamics */
    MatrixXd B_op;
    /** @brief operational space gravity*/
    VectorXd g_op;
    /** @brief extended jacobian inverse*/
    MatrixXd J_inv;

    /** @brief reference torques */
    VectorXd tau_ref;
    /** @brief nullspace torques */
    VectorXd tau_null;
    /** @brief reference acceleration in cartesian coordinates */
    Vector3d ddx_ref;
};