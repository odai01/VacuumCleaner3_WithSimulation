cmake_minimum_required(VERSION 3.13)

# Define the project name and C++ standard
project(Algo_Spiral VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add Spiral algorithm as a shared library
add_library(212609440_322776063_SpiralCleaningAlgorithm SHARED 212609440_322776063_SpiralCleaningAlgorithm.cpp ../../common/Logger.cpp)

# Include the common directory for headers
target_include_directories(212609440_322776063_SpiralCleaningAlgorithm PUBLIC ${CMAKE_SOURCE_DIR}/../../common)

# Set the output directory for the .so file
set_target_properties(212609440_322776063_SpiralCleaningAlgorithm PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/algorithm)
