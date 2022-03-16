#pragma once

#include "3d-soft-trunk/ControllerPCC.h"

class Characterize: public ControllerPCC {
public:
    Characterize(const SoftTrunkParameters st_params);

    /** @brief log a graph containing radial intensity of arm's actuation
     * @details the arm doesn't actuate equally in all directions, this is meant to enable counteracting that
     * this takes very long to execute (30 minutes) as it is very precise
     * @param segment segment characterized, starts at 0 (base) */
    void angularError(int segment, std::string filename);

    /** @brief calculate optimal coefficients for gravity vs K term using least squares fitting*/
    void stiffness(int segment, int verticalsteps = 2, int maxpressure = 500);

    bool valveMap(int maxpressure = 500);

    void actuation(int segment, int points, int maxpressure);

    std::string yaml_name_ = "defaultCharacterizer.yaml";

    SoftTrunkParameters new_params{};

private:
    const double deg2rad = 0.01745329;
};