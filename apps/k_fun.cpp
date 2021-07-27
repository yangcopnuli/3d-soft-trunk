//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: k_fun.cpp
//
// MATLAB Coder version            : 5.1
// C/C++ source code generated on  : 27-Jul-2021 20:24:07
//

// Include Files
#include "k_fun.h"

// Function Definitions
//
// K_FUN
//     K = k_fun(k_vect,q);
// Arguments    : const double in1[2]
//                const double in2[4]
//                double K[4]
// Return Type  : void
//
void k_fun(const double in1[2], const double in2[4], double K[4])
{
  //     This function was generated by the Symbolic Math Toolbox version 8.7.
  //     26-Jul-2021 21:32:53
  K[0] = 0.0;
  K[1] = in1[0] * in2[1];
  K[2] = 0.0;
  K[3] = in1[1] * in2[3];
}

int main(){
  double k_vect[2] = {0.125, 0.125};
  double q[4] = {0.3, 0.5, 0.3, 0.5};
  double K[4];
  k_fun(k_vect,q,K);
  Eigen::VectorXd stiffness = Eigen::VectorXd::Zero(4);
  for (int i = 0; i < 4; i++){
      stiffness(i) = K[i];
    }
  std::cout << stiffness << std::endl;
}
//
// File trailer for k_fun.cpp
//
// [EOF]
//