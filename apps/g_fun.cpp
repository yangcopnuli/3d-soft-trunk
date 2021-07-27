//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: g_fun.cpp
//
// MATLAB Coder version            : 5.1
// C/C++ source code generated on  : 27-Jul-2021 19:49:03
//

// Include Files
#include "g_fun.h"
#include <cmath>
#include <iostream>
#include <Eigen/Dense>
// Function Definitions
//
// G_FUN
//     g0 = 9.81;
//  G = g_fun(q,L,m,g0);
// Arguments    : const double in1[4]
//                const double in2[2]
//                const double in3[2]
//                double g0
//                double G[4]
// Return Type  : void
//
void g_fun(const double in1[4], const double in2[2], const double in3[2], double
           g0, double G[4])
{
  double G_tmp;
  double b_G_tmp;
  double c_G_tmp;
  double d_G_tmp;
  double e_G_tmp;
  double t10;
  double t12;
  double t16;
  double t16_tmp;
  double t2;
  double t3;
  double t4;
  double t5;
  double t6;
  double t7;
  double t8;
  double t9;

  //     This function was generated by the Symbolic Math Toolbox version 8.7.
  //     26-Jul-2021 21:32:53
  t2 = std::cos(in1[0]);
  t3 = std::cos(in1[1]);
  t4 = std::cos(in1[2]);
  t5 = std::cos(in1[3]);
  t6 = std::sin(in1[0]);
  t7 = std::sin(in1[1]);
  t8 = std::sin(in1[2]);
  t9 = std::sin(in1[3]);
  t10 = in1[1] * in1[1];
  t12 = 1.0 / in1[3];
  t16_tmp = in2[1] * g0 * in3[1];
  t16 = t16_tmp * t7 * t12 * (t5 - 1.0) * std::sin(in1[0] + -in1[2]) / 2.0;
  G[0] = t16;
  G_tmp = in2[0] * in3[0];
  b_G_tmp = in2[0] * in3[1];
  c_G_tmp = in2[1] * in3[1];
  d_G_tmp = c_G_tmp * t2 * t3 * t4;
  e_G_tmp = c_G_tmp * t3;
  G[1] = g0 * t12 * ((((((((G_tmp * in1[3] * t7 + b_G_tmp * in1[3] * t7 * 2.0) -
    G_tmp * in1[1] * in1[3] * t3) - b_G_tmp * in1[1] * in1[3] * t3 * 2.0) +
    c_G_tmp * t7 * t9 * t10) + d_G_tmp * t10) + e_G_tmp * t6 * t8 * t10) -
                      d_G_tmp * t5 * t10) - e_G_tmp * t5 * t6 * t8 * t10) / (t10
    * 2.0);
  G[2] = -t16;
  G_tmp = t2 * t4;
  G[3] = t16_tmp * (t12 * t12) * (((((((t3 * t9 - in1[3] * t3 * t5) - G_tmp * t7)
    - t6 * t7 * t8) + G_tmp * t5 * t7) + t5 * t6 * t7 * t8) + in1[3] * t2 * t4 *
    t7 * t9) + in1[3] * t6 * t7 * t8 * t9) / 2.0;
}

int main(){
  double q[4] = {0.3, 0.4, 0.3, 0.5};
  double dq[4] = {0.5, 0.3, 0.2, 0.1};
  double L[2] = {0.125, 0.125};
  double m[2] = {0.16, 0.082};
  double g[4];
  
  g_fun(q,L,m,g0,g);
  Eigen::VectorXd G = Eigen::VectorXd::Zero(4);
  for (int i = 0; i < 4; i++){
      G(i) = g[i];
    }
  std::cout << G << std::endl;
}
//
// File trailer for g_fun.cpp
//
// [EOF]
//