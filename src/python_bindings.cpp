#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <3d-soft-trunk/AugmentedRigidArm.h>
#include <3d-soft-trunk/CurvatureCalculator.h>
#include <3d-soft-trunk/SoftTrunkModel.h>

namespace py = pybind11;

PYBIND11_MODULE(softtrunk_pybind_module, m){
    py::class_<AugmentedRigidArm>(m, "AugmentedRigidArm")
    .def(py::init<>())
    .def("update", &AugmentedRigidArm::update)
    .def("get_H", [](AugmentedRigidArm& ara, int i){
        return ara.get_H(i).matrix();
    })
    .def("get_H_tip", [](AugmentedRigidArm& ara){
        return ara.get_H_tip().matrix();
    });

    py::class_<CurvatureCalculator> cc(m, "CurvatureCalculator");
    cc.def(py::init<CurvatureCalculator::SensorType, std::string>())
    .def("get_curvature", [](CurvatureCalculator& cc){
        // return as tuple rather than by reference
        Pose pose;
        cc.get_curvature(pose);
        return std::make_tuple(pose.q, pose.dq, pose.ddq);
    })
    .def("get_frame", [](CurvatureCalculator& cc, int i){
        Eigen::Matrix<double, 4, 4> H = Eigen::Matrix<double, 4, 4>::Identity();
        H = cc.get_frame(i).matrix();
        return H;
    })
    .def("get_timestamp", &CurvatureCalculator::get_timestamp);
    
    py::class_<SoftTrunkModel> stm(m, "SoftTrunkModel");
    stm.def(py::init<>())
    .def("updateState", &SoftTrunkModel::updateState)
    .def("getModel", [](SoftTrunkModel& stm){
        return std::make_tuple(stm.B, stm.c, stm.g, stm.K, stm.D, stm.A, stm.J);
    });

    py::enum_<CurvatureCalculator::SensorType>(cc, "SensorType")
    .value("qualisys", CurvatureCalculator::SensorType::qualisys)
    .value("bend_labs", CurvatureCalculator::SensorType::bend_labs);
}
