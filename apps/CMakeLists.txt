### examples

add_executable(example_Simulator example_Simulator.cpp)
target_link_libraries(example_Simulator ControllerPCC)

add_executable(example_AugmentedRigidArm example_AugmentedRigidArm.cpp)
target_link_libraries(example_AugmentedRigidArm AugmentedRigidArm)

add_executable(example_Characterizer example_Characterizer.cpp)
target_link_libraries(example_Characterizer Characterizer)

add_executable(example_Model example_Model.cpp)
target_link_libraries(example_Model Model)

add_executable(example_StateEstimator example_StateEstimator.cpp)
target_link_libraries(example_StateEstimator StateEstimator Model)

add_executable(example_Controller example_Controller.cpp)
target_link_libraries(example_Controller ControllerPCC)

if(${roscpp_FOUND})
    add_executable(example_VisualizerROS example_VisualizerROS.cpp)
    target_link_libraries(example_VisualizerROS SoftTrunkModel VisualizerROS)
endif(${roscpp_FOUND})



### other programs (experiments etc.)

add_executable(example_OSC example_OSC.cpp)
target_link_libraries(example_OSC ControllerPCC OSC)

add_executable(simulate_logged_pressure simulate_logged_pressure.cpp)
target_link_libraries(simulate_logged_pressure ControllerPCC Threads::Threads)

add_executable(fullCharacterize fullCharacterize.cpp)
target_link_libraries(fullCharacterize Characterizer)

if(${roscpp_FOUND})
    add_executable(ui_controller ui_controller.cpp)
    target_link_libraries(ui_controller SoftTrunkModel OSC VisualizerROS)
endif(${roscpp_FOUND})