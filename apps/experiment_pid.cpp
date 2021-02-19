//
// Created by yasu on 25/12/2020.
//

#include "3d-soft-trunk/ControllerPCC.h"

/**
 * @brief run a PID controller
 */
int main(){
    VectorXd q_ref = VectorXd::Zero(st_params::num_segments * 2);
    VectorXd dq_ref = VectorXd::Zero(st_params::num_segments * 2);
    VectorXd ddq_ref = VectorXd::Zero(st_params::num_segments * 2);
    for (int i = 0; i < st_params::num_segments; i++) {
        q_ref(2 * i) = 0.15;
        q_ref(2 * i + 1) = 0.15;
    }
    ControllerPCC cpcc{CurvatureCalculator::SensorType::qualisys};
    cpcc.set_ref(q_ref, dq_ref, ddq_ref);

    srl::Rate r{5};
    VectorXd q, dq, p_vectorized;
    while (true) {
        // the controller is updated at a much higher rate in a separate thread, this is just to show the output in realtime
        cpcc.get_kinematic(q, dq);
        cpcc.get_pressure(p_vectorized);
        fmt::print("--------\n");
        fmt::print("q_meas:\t{}\n", q.transpose());
        fmt::print("q_ref:\t{}\n", q_ref.transpose());
        fmt::print("p_vec:\t{}\n", p_vectorized.transpose());
        r.sleep();
    }

}