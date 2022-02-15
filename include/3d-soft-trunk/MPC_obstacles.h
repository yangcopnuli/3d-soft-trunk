#pragma once

#include "3d-soft-trunk/ControllerPCC.h"
#include <casadi/casadi.hpp>
#include <casadi/core/optistack.hpp>
#include <chrono>
using namespace casadi;

class MPC_obstacles: public ControllerPCC{
    public:
        MPC_obstacles(const SoftTrunkParameters st_params, CurvatureCalculator::SensorType sensor_type);
        void set_ref(const MatrixXd refx);
        const int Horizon = 7;  // 5 original, 7 to have good tracking but unstable
                                // with horizon 7 the terminal constraint doesn't affect the reachability

    protected:
        MatrixXd traj_ref = MatrixXd::Zero(3, Horizon+1);
        MatrixXd dtraj_ref = MatrixXd::Zero(3, Horizon+1);
        MatrixXd ddtraj_ref = MatrixXd::Zero(3, Horizon+1);

    private:
        void control_loop(); 
        Opti define_problem();
        void get_state_space(MatrixXd B, MatrixXd c, MatrixXd g, MatrixXd K, MatrixXd D, MatrixXd A, MatrixXd &sp_A, MatrixXd &sp_B, MatrixXd &sp_w, double Ts);
        int solveDARE(MatrixXd A, MatrixXd B, MatrixXd Q, MatrixXd R, MatrixXd &X);   // from https://github.com/wcaarls/grl/blob/master/addons/lqr/src/lqr.cpp
        MatrixXd matrix_exponential(MatrixXd A, int size); 
        MX Rotx(MX theta);
        MX Roty(MX theta);
        MX ee_position(MX thetax, MX thetay, MX length1, MX length2);
        DM robust_correction(DM U);
        
        VectorXd p_prev = VectorXd::Zero(2*st_params.num_segments);
        VectorXd tau_ref;

        
        Opti ctrl;
        bool solved;  
        //OptiSol sol; 

        MX q;
        MX q_dot; 
        MX A;  // state-space
        MX B;
        MX w;  // additional terms not in state-space
        MX u; 
        MX x_r; 
        MX tr_r; 
        MX q_0;
        MX q_dot_0;
        MX u_prev; 

        DM u_temp; // input placeholder
        DM u_robust; 
        DM q_old; 
        DM q_dot_old; 
        MatrixXd p_temp = MatrixXd::Zero(2*st_params.num_segments,1);
        VectorXd pxy;

        MatrixXd sp_A = MatrixXd::Zero(2*st_params.q_size, 2*st_params.q_size);
        MatrixXd sp_B = MatrixXd::Zero(2*st_params.q_size, 2*st_params.num_segments);   // SS matrices
        MatrixXd sp_w = MatrixXd::Zero(2*st_params.q_size, 1); 

        DM sp_A_temp = DM::nan(2*st_params.q_size, 2*st_params.q_size);
        DM sp_B_temp = DM::nan(2*st_params.q_size, 2*st_params.num_segments);
        DM sp_w_temp = DM::nan(2*st_params.q_size, 1); 
        DM K_temp = DM::nan(2*st_params.num_segments, 2*st_params.q_size);

        DM x_r_temp = DM::nan(3,1);  // conversion placeholders
        DM tr_r_temp = DM::nan(3,Horizon+1); 
        DM q_0_temp = DM::nan(st_params.q_size, 1);
        DM q_dot_0_temp = DM::nan(st_params.q_size, 1);
        DM q_0_large = DM::nan(st_params.q_size, Horizon+1);    // needed for warm-start
        DM q_dot_0_large = DM::nan(st_params.q_size, Horizon+1); 
        DM u_large = DM::nan(2*st_params.num_segments, Horizon);

        MatrixXd sp_A_c = MatrixXd::Zero(2*st_params.q_size, 2*st_params.q_size);
        MatrixXd sp_B_c = MatrixXd::Zero(2*st_params.q_size, 2*st_params.num_segments); 
        MatrixXd sp_w_c = MatrixXd::Zero(2*st_params.q_size, 1);
        MatrixXd Ad; 
        int length; 

        MX rot; 
        MX len1;
        MX len2;
        MX totRot;
        MX inter = MX::zeros(3,1);

        DM disturbance = DM::zeros(2*st_params.q_size, 1);

        MatrixXd P, P_old; 
        MatrixXd R = 1e-4*MatrixXd::Identity(2*st_params.num_segments, 2*st_params.num_segments);
        MatrixXd Q = MatrixXd::Zero(2*st_params.q_size, 2*st_params.q_size);
        MatrixXd K;
        int res; 

        DM v_temp; 
        DM x_temp;

        MatrixXd At, Bt; 

        int counter = 0; 
        double total_time = 0;
        VectorXd slow_execution = VectorXd::Zero(5); 
};