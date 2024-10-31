cmake_minimum_required(VERSION 3.13)

# Define the project name and set the C++ standard to C++20
project(robot_vacuum_simulator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Add Spiral and DFS algorithms as shared libraries
add_subdirectory(algorithm/Algo_Spiral)
add_subdirectory(algorithm/Algo_DFS)

# Add the simulator executable
add_executable(simulator 
    simulator/AlgorithmRegistrar.cpp 
    simulator/mySimulator.cpp
    common/ConcreteWallSensor.cpp
    common/ConcreteDirtSensor.cpp
    common/ConcreteBatteryMeter.cpp
    common/Logger.cpp  # Add Logger.cpp here
    simulator/main.cpp)  # Added main.cpp to the executable

# Ensure dynamic linking and include directories
target_link_options(simulator PUBLIC "-rdynamic")
target_include_directories(simulator PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(simulator dl 212609440_322776063_SpiralCleaningAlgorithm 212609440_322776063_DFS pthread stdc++fs)
