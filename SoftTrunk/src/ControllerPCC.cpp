//
// Created by yasu and rkk on 26/10/18.
//

#include "ControllerPCC.h"

ControllerPCC::ControllerPCC(AugmentedRigidArm* augmentedRigidArm, SoftArm* softArm) : ara(augmentedRigidArm), sa(softArm){

}

ControllerPCC::ControllerPCC(SoftArm* softArm) : sa(softArm){
    for (int i = 0; i < NUM_ELEMENTS; ++i) {
        miniPIDs.push_back(MiniPID(0,0,0)); // PID for phi
        miniPIDs.push_back(MiniPID(150,0.2,0)); // PID for theta
    }
}

void ControllerPCC::curvatureDynamicControl(const Vector2Nd &q_ref,
                                            const Vector2Nd &dq_ref,
                                            const Vector2Nd &ddq_ref,
                                            Vector2Nd *tau) {
    curvatureDynamicControl(sa->curvatureCalculator->q, sa->curvatureCalculator->dq, q_ref, dq_ref, ddq_ref, tau);
}

void ControllerPCC::curvatureDynamicControl(
        const Vector2Nd &q_meas,
        const Vector2Nd &dq_meas,
        const Vector2Nd &q_ref,
        const Vector2Nd &dq_ref,
        const Vector2Nd &ddq_ref,
        Vector2Nd *tau) {
    ara->update(q_meas, dq_meas);
    B = ara->Jm.transpose() * ara->B_xi * ara->Jm;
    C = ara->Jm.transpose() * ara->B_xi * ara->dJm;
    G = ara->Jm.transpose() * ara->G_xi;
    *tau = sa->k.asDiagonal()*q_ref + sa->d.asDiagonal()*dq_ref + G + C*dq_ref + B*ddq_ref;
}

void ControllerPCC::curvaturePIDControl(const Vector2Nd &q_ref, Vector2Nd *output) {
    for (int i = 0; i < 2 * NUM_ELEMENTS; ++i) {
        (*output)(i) = miniPIDs[i].getOutput(sa->curvatureCalculator->q(i), q_ref[i]);
    }
}