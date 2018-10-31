//
// Created by yasu on 26/10/18.
//

#ifndef SOFTTRUNK_CONTROLLERPCC_H
#define SOFTTRUNK_CONTROLLERPCC_H

#include "SoftTrunk_common_defs.h"
#include <Eigen/Dense>
#include <AugmentedRigidArm.h>



class ControllerPCC{
    /*
     * Implements the PCC controller as described in paper.
     */
public:
    ControllerPCC(AugmentedRigidArm *); // pass on k and d and pointer to AugmentedRigidArm
    void curvatureDynamicControl(
            const Vector2Nd &q_meas,
            const Vector2Nd &dq_meas,
            const Vector2Nd &q_ref,
            const Vector2Nd &dq_ref,
            const Vector2Nd &ddq_ref,
            Vector2Nd *tau); // pass on the measured & reference values, to get tau.
private:
  Matrix2Nd B;
  Matrix2Nd C;
  Vector2Nd G;
  Vector2Nd k;
  Vector2Nd d;
  AugmentedRigidArm *ara;
};



#endif //SOFTTRUNK_CONTROLLERPCC_H
