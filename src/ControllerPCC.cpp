//
// Created by yasu and rkk on 26/10/18.
// revamped by oliver in may 21
//

#include "3d-soft-trunk/ControllerPCC.h"



ControllerPCC::ControllerPCC(const SoftTrunkParameters st_params) : st_params_(st_params){
    assert(st_params_.is_finalized());
    // set appropriate size for each member
    state_.setSize(st_params_.q_size);
    state_prev_.setSize(st_params_.q_size);
    state_ref_.setSize(st_params_.q_size);
    p_ = VectorXd::Zero(st_params_.p_size);
    f_ = VectorXd::Zero(st_params_.q_size);

    filename_ = "defaultController_log";

    mdl_ = std::make_unique<Model>(st_params_);
    ste_ = std::make_unique<StateEstimator>(st_params_);

    vc_ = std::make_unique<ValveController>("192.168.0.100", st_params_.valvemap, p_max);

    fmt::print("ControllerPCC object initialized.\n");
}

void ControllerPCC::set_ref(const srl::State &state_ref) {
    std::lock_guard<std::mutex> lock(mtx);
    // assign to member variables
    this->state_ref_ = state_ref;
    if (!is_initial_ref_received)
        is_initial_ref_received = true;
}

void ControllerPCC::set_ref(const Vector3d &x_ref, const Vector3d &dx_ref, const Vector3d &ddx_ref){
    std::lock_guard<std::mutex> lock(mtx);

    this->x_ref_ = x_ref;
    this->dx_ref_ = dx_ref;
    this->ddx_ref_ = ddx_ref;
    if (!is_initial_ref_received)
        is_initial_ref_received = true;
}

void ControllerPCC::toggleGripper(){
    assert(gripperAttached_);
    gripping_ = !gripping_;
    vc_->setSinglePressure(3*st_params_.num_segments, gripping_*350); //350mbar to grip
}

VectorXd ControllerPCC::gravity_compensate(const srl::State state){
    assert(st_params_.sections_per_segment == 1);
    VectorXd gravcomp = dyn_.A_pseudo.inverse() * (dyn_.g + dyn_.K * state.q + dyn_.D * state.dq + dyn_.c);

    return gravcomp/100; //to mbar
}

void ControllerPCC::actuate(const VectorXd &p) { //actuates valves according to mapping from header
    assert(p.size() == st_params_.p_size);
    for (int i = 0; i < st_params_.p_size; i++){
        vc_->setSinglePressure(i, p(i));
    }
    if (logging_){  
        log(state_.timestamp/10e6);                     //log once per control timestep
    }
}


bool ControllerPCC::simulate(const VectorXd &p){
    mdl_->update(state_);
    this->dyn_ = mdl_->dyn_;
    state_prev_.ddq = state_.ddq;
    VectorXd p_adjusted = 100*p; //convert from mbar

    VectorXd b_inv_rest = dyn_.B.inverse() * (dyn_.A * p_adjusted - dyn_.c - dyn_.g - dyn_.K * state_.q);      //set up constant terms to not constantly recalculate
    MatrixXd b_inv_d = -dyn_.B.inverse() * dyn_.D;
    VectorXd ddq_prev;

    for (int i=0; i < int (dt_/0.00001); i++){                                              //forward integrate dq with high resolution
        ddq_prev = state_.ddq;
        state_.ddq = b_inv_rest + b_inv_d*state_.dq;
        state_.dq += 0.00001*(2*(2*state_.ddq - ddq_prev) + 5*state_.ddq - ddq_prev)/6;  
    }

    state_.q = state_.q + state_.dq*dt_ + (dt_*dt_*(4*state_.ddq - state_prev_.ddq) / 6);


    if (logging_){                                               //log once per control timestep
        log(t_);
    }
    t_+=dt_;

    return !(abs(state_.ddq[0])>pow(10.0,10.0) or abs(state_.dq[0])>pow(10.0,10.0) or abs(state_.q[0])>pow(10.0,10.0)); //catches when the sim is crashing, true = all ok, false = crashing
}

void ControllerPCC::toggle_log(){
    if(!logging_) {
        logging_ = true;
        if (!(st_params_.sensors[0] == SensorType::simulator)) {initial_timestamp_ = state_.timestamp;}
        else {initial_timestamp_ = 0;}
        this->filename_ = fmt::format("{}/{}.csv", SOFTTRUNK_PROJECT_DIR, filename_);
        fmt::print("Starting log to {}\n", this->filename_);
        log_file_.open(this->filename_, std::fstream::out);
        log_file_ << "timestamp";

        //write header
        log_file_ << fmt::format(", x, y, z, x_ref, y_ref, z_ref, err");

        for (int i=0; i < st_params_.q_size; i++)
            log_file_ << fmt::format(", q_{}", i);
        for (int i=0; i < st_params_.p_size; i++)
            log_file_ << fmt::format(", p_{}", i);

        log_file_ << "\n";
    } else {
        logging_ = false;
        fmt::print("Ending log to {}\n", this->filename_);
        log_file_.close();
    }
}

void ControllerPCC::log(double time){
    Vector3d x_tip = Vector3d::Zero();

    if (st_params_.sensors[0] == SensorType::qualisys){
        x_tip = state_.tip_transforms[st_params_.prismatic].rotation()*(state_.tip_transforms[st_params_.num_segments+st_params_.prismatic].translation()-state_.tip_transforms[st_params_.prismatic].translation());
    }

    log_file_ << fmt::format(", {}, {}, {}, {}, {}, {}, {}", x_tip(0), x_tip(1), x_tip(2), x_ref_(0), x_ref_(1), x_ref_(2), (x_tip - x_ref_).norm());

    for (int i=0; i < st_params_.q_size; i++)               //log q
        log_file_ << fmt::format(", {}", state_.q(i));
    for (int i=0; i < st_params_.p_size; i++)
        log_file_ << fmt::format(", {}", p_(i));
    log_file_ << "\n";
}