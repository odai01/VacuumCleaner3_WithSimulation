#ifndef MY_SIMULATOR_H
#define MY_SIMULATOR_H

#include <vector>
#include <memory>
#include <string>
#include <utility>
#include <thread>
#include "../algorithm/Algo_Spiral/212609440_322776063_SpiralCleaningAlgorithm.h"
#include "../algorithm/Algo_DFS/212609440_322776063_DFS.h"
#include "../common/AlgorithmRegistrar.h"
#include <mutex>
#include <dlfcn.h>
#include <fstream>
#include <set>
#include <algorithm>
#include "../common/ConcreteWallSensor.h"
#include "../common/ConcreteDirtSensor.h"
#include "../common/ConcreteBatteryMeter.h"
#include "../common/AbstractAlgorithm.h"
#include "../common/Logger.h"

class MySimulator {

public:
    void run(int argc, char** argv); // Corrected run method signature

private:
    // Struct to hold the simulation results for each house-algorithm pair
    struct SimulationResult {
        std::string houseName;
        std::string algorithmName;
        int numSteps;
        int dirtLeft;
        bool inDock;
        std::string status; // "DEAD", "FINISHED", or "WORKING"
        int score;
        std::vector<std::tuple<int, int>> stepsHistory;
    };

    // Struct to manage the loaded algorithms
    struct AlgorithmHandle {
        std::string name;
        void* handle;
        std::unique_ptr<AbstractAlgorithm> instance;

        void resetInstance() {
            instance.reset();
            AlgorithmRegistrar::getAlgorithmRegistrar().clear();
        }
    };

    

    Logger simulatorLogger{"simulator.log"};
    AbstractAlgorithm* algorithm;
    int initialDirtLevel;
    std::string houseName;
    std::string inputFileName;
    std::size_t maxSteps;
    std::size_t maxBattery;
    std::size_t rows;
    std::size_t cols;
    std::tuple<int, int> dockingStation;
    std::tuple<int, int> currentPosition;
    std::vector<SimulationResult> simulationResults; // Store the results of each simulation
    std::mutex resultsMutex; // Protect access to simulationResults
    std::mutex houseMutex;
    std::mutex algoMutex;
    bool readHouseFile(const std::string& houseFilePath, std::vector<std::vector<char>>& house);
    void setAlgorithm(AbstractAlgorithm& algo, std::vector<std::vector<char>>& house, std::tuple<int, int>& dockingStation, std::unique_ptr<ConcreteWallSensor>& wallsSensor, std::unique_ptr<ConcreteDirtSensor>& dirtSensor, std::unique_ptr<ConcreteBatteryMeter>& batteryMeter);
    void loadAndRunSimulations(const std::string& housePath, const std::string& algoPath, int numThreads);
    void runSimulations(const std::vector<std::string>& houseFiles, const std::vector<AlgorithmHandle>& algorithms, int numThreads);
    void runSimulation(const std::string& algorithmName, const std::string& houseName, 
                                std::vector<std::vector<char>> houseCopy, AbstractAlgorithm& algo, 
                                std::unique_ptr<ConcreteWallSensor>& wallsSensor, 
                                std::unique_ptr<ConcreteDirtSensor>& dirtSensor, 
                                std::unique_ptr<ConcreteBatteryMeter>& batteryMeter);
    int calculateScore(int maxSteps, int numSteps, int dirtLeft, bool inDock, const std::string& status);
    void generateSummaryCSV(const std::vector<SimulationResult>& results);
    void writeSimulationOutput(const SimulationResult& result);
    char calculateDirectionFromSteps(const std::tuple<int, int>& previousPosition, const std::tuple<int, int>& currentPosition);
    void writeHouseMatrix(const std::string& filename, const std::vector<std::tuple<std::string, std::vector<std::vector<char>>, int, int>>& houses);
    void writeStepsHistory(const std::string& filename, const std::vector<std::tuple<std::string, std::string, std::vector<std::tuple<int, int>>, std::tuple<int, int>, int>>& simulationResults);
};

#endif // MY_SIMULATOR_H
