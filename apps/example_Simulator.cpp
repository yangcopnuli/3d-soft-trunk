#include <3d-soft-trunk/SoftTrunkModel.h>
#include <3d-soft-trunk/Simulator.h>
#include <chrono>

int main(){


    SoftTrunkModel stm = SoftTrunkModel();
    srl::State state;
    VectorXd p = VectorXd::Zero(3*st_params::num_segments);
    double control_step = 0.01;
    double time = 4.0;
    
    for (int i = 0; i < st_params::num_segments; i++) {
        // set to have about the same curvature as a whole regardless of scale
        double rand = -2.093 / st_params::sections_per_segment / st_params::num_segments;
        for (int j = 0; j < st_params::sections_per_segment; j++){
            state.q(2*i*st_params::sections_per_segment + 2*j + 1) = -rand;
            state.q(2*i*st_params::sections_per_segment + 2*j ) = rand * 0.022 / 0.19;
        }
            
    }
    

    Simulator sim = Simulator(stm, control_step, 1);

    sim.start_log("sim_timestep10ms");

    auto start = std::chrono::steady_clock::now();

    for (double t=0; t < time; t+=control_step){
        if (!sim.simulate(p, state)){
            std::cout << "Sim crashed! Returning...\n";
            break;
        }
    }

    sim.end_log();

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Simulated " << time << "s of motion in " << elapsed.count() <<"s realtime using timestep " << control_step << "\n";
}