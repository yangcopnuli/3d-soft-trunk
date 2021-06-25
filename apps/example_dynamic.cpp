#include "3d-soft-trunk/Dyn.h"

int main(){
    srl::State state_ref;
    Dyn d(CurvatureCalculator::SensorType::qualisys, false); 
    state_ref.q << -0.261782868,	-0.669757892,	0.707805025,	-0.390006514;
    d.set_ref(state_ref);
    while(true){
        getchar();
        d.toggle_log();
        srl::sleep(5);
        d.toggle_log();
    };
}