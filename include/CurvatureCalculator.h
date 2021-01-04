#pragma once

#include "mobilerack-interface/QualisysClient.h"
#include "mobilerack-interface/SerialInterface.h"
#include "SoftTrunk_common.h"

#include <Eigen/Geometry>
#include <cmath>
#include <fstream>
#include <thread>
#include <cmath>
#include <mutex>
enum class SensorType{
    qualisys,
    bend_labs
};
/**
 * @brief Calculates the PCC configuration of each soft arm segment based on motion track / internal sensor measurement.
 * @details For Qualisys mode, the frames have to be set up properly in QTM (using Rigid Body) first.
 * Rigid Body label conventions: base of robot is 0, tip of first segment is 1, and so on...
 * Z axis of each frame is parallel to lengthwise direction of the arm, and origin of each frame is at center of tip of segment
 * @todo cannot gracefully deal with missed frames etc when there are occlusions.
 */
class CurvatureCalculator {
private:
    SensorType sensor_type;

    std::unique_ptr<QualisysClient> optiTrackClient;
    /**
     * @brief recorded data from motion tracking system. Saves absolute transforms for each frame.
     */
    std::vector<Eigen::Transform<double, 3, Eigen::Affine>> abs_transforms;
    const char *qtm_address = "192.168.120.97";
    
    std::unique_ptr<SerialInterface> serialInterface;
    std::vector<float> bendLab_data;
    std::string serialPort = "/dev/cu.usbmodem14201";

    unsigned long long int timestamp;
    
    std::thread calculatorThread;
    
    /**
     * @brief background process that calculates curvature
     */
    void calculator_loop();

    /**
     * @brief thread runs while this is true
     */
    bool run;
    const bool log = true;

    /**
     * @brief calculates q from the current frame values.
     */
    void calculateCurvature();

    VectorXd initial_q;
    std::mutex mtx;

    /** @brief PCC configuration of each segment of soft arm. depends on st_params::parametrization */
    VectorXd q;
    VectorXd dq;
    VectorXd ddq;

public:
    CurvatureCalculator(SensorType sensor_type = SensorType::qualisys);

    // @todo is this actually called at the end??
    ~CurvatureCalculator();

    void setupQualisys();

    /**
     * @brief for future, if you want to use sensors embedded in arm.
     */
    void setupIntegratedSensor();

    void get_curvature(VectorXd &q, VectorXd &dq, VectorXd &ddq);
};
