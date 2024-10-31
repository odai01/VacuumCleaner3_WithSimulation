#include "mySimulator.h"



#include <sstream>

#include <iostream>

#include <filesystem>
#include <future>



bool MySimulator::readHouseFile(const std::string& houseFilePath, std::vector<std::vector<char>>& house) {

    std::ifstream file(houseFilePath);

    if (!file) {

        std::cerr << "Error: Could not open file " << houseFilePath << std::endl;

        return false;

    }



    inputFileName = houseFilePath;

    std::string line;

    std::getline(file, houseName); // Line 1: house name / description



    std::getline(file, line); // Line 2: MaxSteps

    std::stringstream(line.substr(line.find('=') + 1)) >> maxSteps;



    std::getline(file, line); // Line 3: MaxBattery

    std::stringstream(line.substr(line.find('=') + 1)) >> maxBattery;



    std::getline(file, line); // Line 4: Rows

    std::stringstream(line.substr(line.find('=') + 1)) >> rows;



    std::getline(file, line); // Line 5: Cols

    std::stringstream(line.substr(line.find('=') + 1)) >> cols;



    // Read the house structure

    bool DockingFound = false;

    initialDirtLevel = 0;

    house = std::vector<std::vector<char>>(rows, std::vector<char>(cols, ' '));

    for (std::size_t i = 0; i < rows; ++i) {

        if (std::getline(file, line)) {

            for (std::size_t j = 0; j < line.size() && j < cols; ++j) {

                house[i][j] = line[j];

                if (line[j] == 'D') {

                    DockingFound = true;

                    dockingStation = {i, j};

                    currentPosition = dockingStation;

                } else if (line[j] >= '1' && line[j] <= '9') {

                    initialDirtLevel += line[j] - '0'; // Sum the dirt levels

                }

            }

        }

    }



    if (!DockingFound) {

        std::cerr << "Error: No Docking Station Was Found in " << houseFilePath << std::endl;

        return false;

    }



    std::cout << "Successfully read house file: " << houseFilePath << std::endl;

    return true;

}



void MySimulator::setAlgorithm(AbstractAlgorithm& algo, std::vector<std::vector<char>>& house, std::tuple<int, int>& dockingStation, 

                                std::unique_ptr<ConcreteWallSensor>& wallsSensor, std::unique_ptr<ConcreteDirtSensor>& dirtSensor, 

                                std::unique_ptr<ConcreteBatteryMeter>& batteryMeter) {



    algo.setMaxSteps(maxSteps);



    int x = std::get<0>(dockingStation);

    int y = std::get<1>(dockingStation);



    // Ensure sensors are initialized based on the actual state of the house at the robot's initial position

    bool northWall = (y == 0 || house[x][y - 1] == 'W');

    bool eastWall = (x == rows - 1 || house[x + 1][y] == 'W');

    bool southWall = (y == cols - 1 || house[x][y + 1] == 'W');

    bool westWall = (x == 0 || house[x - 1][y] == 'W');



    wallsSensor = std::make_unique<ConcreteWallSensor>(northWall, eastWall, southWall, westWall);

    dirtSensor = std::make_unique<ConcreteDirtSensor>(house[x][y] - '0');

    batteryMeter = std::make_unique<ConcreteBatteryMeter>(maxBattery);



    algo.setWallsSensor(*wallsSensor);

    algo.setDirtSensor(*dirtSensor);

    algo.setBatteryMeter(*batteryMeter);

}




void MySimulator::run(int argc, char** argv) {

    std::string housePath = "./houses";

    std::string algoPath = "./algorithms";

    int numThreads = 10;



    for (int i = 1; i < argc; ++i) {

        std::string arg = argv[i];

        if (arg.find("-house_path=") == 0) {

            housePath = arg.substr(std::string("-house_path=").length());

        } else if (arg.find("-algo_path=") == 0) {

            algoPath = arg.substr(std::string("-algo_path=").length());

        } else if (arg.find("-num_threads=") == 0) {

            numThreads = std::stoi(arg.substr(std::string("-num_threads=").length()));

        } else {

            std::cerr << "Unknown argument: " << arg << std::endl;

            return;

        }

    }



    std::cout << "Starting simulation with " << numThreads << " threads." << std::endl;

    loadAndRunSimulations(housePath, algoPath, numThreads);

}



void MySimulator::loadAndRunSimulations(const std::string& housePath, const std::string& algoPath, int numThreads) {
    std::vector<std::string> houseFiles;
    std::vector<AlgorithmHandle> algorithms;

    for (const auto& entry : std::filesystem::directory_iterator(housePath)) {
        if (entry.path().extension() == ".house") {
            houseFiles.push_back(entry.path().string());
            std::cout << "Loaded house file: " << entry.path().string() << std::endl;
        }
    }

    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    std::size_t initialCount = registrar.count();

    for (const auto& entry : std::filesystem::directory_iterator(algoPath)) {
        if (entry.path().extension() == ".so") {
            void* handle = dlopen(entry.path().c_str(), RTLD_LAZY);
            if (!handle) {
                std::cerr << "Cannot open library: " << dlerror() << std::endl;
                continue;
            }
            dlerror();

            if (registrar.count() > initialCount) {
                for (std::size_t i = initialCount; i < registrar.count(); ++i) {
                    auto& algorithmPair = *(registrar.begin() + i);
                    algorithms.push_back(AlgorithmHandle{algorithmPair.name(), handle, algorithmPair.create()});
                    std::cout << "Registered algorithm: " << algorithmPair.name() << std::endl;
                }
                initialCount = registrar.count();
            } else {
                std::cerr << "No new algorithms were registered from: " << entry.path().filename().string() << std::endl;
                dlclose(handle);
            }
        }
    }

    if (algorithms.empty()) {
        std::cerr << "No algorithms loaded. Exiting." << std::endl;
        return;
    }

    std::cout << "Running simulations..." << std::endl;

    runSimulations(houseFiles, algorithms, numThreads);

    for (auto& algoHandle : algorithms) {
        algoHandle.instance.reset();  // Explicitly reset the unique_ptr to delete the algorithm instance
    }

    for (auto& algoHandle : algorithms) {
        if (algoHandle.handle != nullptr) {
            dlclose(algoHandle.handle);  // Close the shared library
            simulatorLogger.log(Logger::INFO, "Closed library for algorithm: " + algoHandle.name);
        }
    }

    simulatorLogger.log(Logger::INFO, "All libraries closed. Simulation run complete.");
}



void MySimulator::runSimulations(const std::vector<std::string>& houseFiles, const std::vector<AlgorithmHandle>& algorithms, int numThreads) {
    std::vector<std::thread> threads;
    std::vector<std::tuple<std::string, std::vector<std::vector<char>>, int, int>> houses;

    {
        std::lock_guard<std::mutex> guard(resultsMutex);
        simulatorLogger.log(Logger::INFO, "Starting simulations with " + std::to_string(houseFiles.size()) + " houses and " + std::to_string(algorithms.size()) + " algorithms, using " + std::to_string(numThreads) + " threads.");
    }

    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();

    for (const auto& houseFile : houseFiles) {
        {
            std::lock_guard<std::mutex> houseGuard(houseMutex);  // Lock each iteration of the external loop

            simulatorLogger.log(Logger::INFO, "Processing house file: " + houseFile);

            for (auto& algoHandle : const_cast<std::vector<AlgorithmHandle>&>(algorithms)) {
                {
                    {
                        //std::lock_guard<std::mutex> houseGuard(resultsMutex);
                        if (threads.size() >= numThreads) {
                            simulatorLogger.log(Logger::INFO, "Joining threads to free up thread pool...");
                            for (auto& thread : threads) {
                                if (thread.joinable()) {
                                    thread.join();
                                }
                            }
                            threads.clear();
                        }

                        if (!algoHandle.instance) {
                            simulatorLogger.log(Logger::ERROR, "Error: Algorithm instance for " + algoHandle.name + " is null!");
                            continue;
                        }
                    
                        /*std::vector<std::vector<char>> houseCopy;
                        if (!readHouseFile(houseFile, houseCopy)) {
                            simulatorLogger.log(Logger::ERROR, "Failed to read house file: " + houseFile);
                            continue;
                        }*/
                        
                        /*for (const auto& pair : registrar) {
                            if (pair.name() == algoHandle.name) {
                                algoHandle.instance = pair.create();
                                break;
                            }
                        }*/

                        simulatorLogger.log(Logger::INFO, "Starting thread for algorithm: " + algoHandle.name + " on house: " + houseFile);

                        threads.emplace_back([houseFile, &algoHandle,registrar, this, &houses]() mutable {
                            try {
                                {
                                    std::lock_guard<std::mutex> guard(resultsMutex);
                                    std::unique_ptr<ConcreteWallSensor> wallsSensor;
                                    std::unique_ptr<ConcreteDirtSensor> dirtSensor;
                                    std::unique_ptr<ConcreteBatteryMeter> batteryMeter;
                                    std::vector<std::vector<char>> houseCopy;
                                    readHouseFile(houseFile, houseCopy);
                                    for (const auto& pair : registrar) {
                                        if (pair.name() == algoHandle.name) {
                                            algoHandle.instance = pair.create();
                                            break;
                                        }
                                    }
                                    this->setAlgorithm(*algoHandle.instance, houseCopy, dockingStation, wallsSensor, dirtSensor, batteryMeter);
                                    this->runSimulation(algoHandle.name, houseFile, houseCopy, *algoHandle.instance, wallsSensor, dirtSensor, batteryMeter);
                                    simulatorLogger.log(Logger::INFO, "Completed simulation for algorithm: " + algoHandle.name + " on house: " + houseFile);
                                    houses.push_back(std::make_tuple(houseFile, houseCopy, maxSteps, maxBattery));   
                                }

                            } catch (const std::exception& e) {
                                std::lock_guard<std::mutex> guard(resultsMutex);
                                simulatorLogger.log(Logger::ERROR, "Exception in thread for algorithm " + algoHandle.name + " on house: " + houseFile + ": " + e.what());
                            } catch (...) {
                                std::lock_guard<std::mutex> guard(resultsMutex);
                                simulatorLogger.log(Logger::ERROR, "Unknown exception in thread for algorithm " + algoHandle.name + " on house: " + houseFile);
                            }
                        });
                        //std::this_thread::sleep_for(std::chrono::seconds(10));
                    }
                }
            }
        }
    }

    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    
    std::vector<SimulationResult> csvResults;

    for (const auto& result : simulationResults) {
        // No need to destructure since `result` is already of type `SimulationResult`
        const std::string& houseName = result.houseName;
        const std::string& algorithmName = result.algorithmName;
        int numSteps = result.numSteps;
        int dirtLeft = result.dirtLeft;
        bool inDock = result.inDock;
        const std::string& status = result.status;
        int score = result.score;

        csvResults.push_back({houseName, algorithmName, numSteps, dirtLeft, inDock, status, score});
    }

    generateSummaryCSV(csvResults);

    std::vector<std::tuple<std::string, std::string, std::vector<std::tuple<int, int>>, std::tuple<int, int>, int>> stepsHistoryData;

    for (const auto& result : simulationResults) {
        // Extract information from the SimulationResult
        const std::string& houseName = result.houseName;
        const std::string& algorithmName = result.algorithmName;
        std::vector<std::tuple<int, int>> stepsHistory = result.stepsHistory;
        std::tuple<int, int> dockingStation = stepsHistory.front();
        int score = result.score;  // Assuming 'score' is part of your SimulationResult

        // Add the extracted data to the new vector
        stepsHistoryData.emplace_back(houseName, algorithmName, stepsHistory, dockingStation, score);
    }

    writeStepsHistory("steps_history.json", stepsHistoryData);
    writeHouseMatrix("initial_house.json", houses);

    {
        std::lock_guard<std::mutex> guard(resultsMutex); // Lock to ensure thread safety when modifying algorithms
        for (auto& algoHandle : const_cast<std::vector<AlgorithmHandle>&>(algorithms)) {
            algoHandle.resetInstance(); // Ensure all algorithms are reset before dlclose

            if (algoHandle.handle != nullptr) {
                dlclose(algoHandle.handle);
                simulatorLogger.log(Logger::INFO, "Closed library for algorithm: " + algoHandle.name);
            }
        }
    }

    simulatorLogger.log(Logger::INFO, "All libraries closed. Simulation run complete.");
}












void MySimulator::runSimulation(const std::string& algorithmName, const std::string& houseName, 
                                std::vector<std::vector<char>> houseCopy, AbstractAlgorithm& algo, 
                                std::unique_ptr<ConcreteWallSensor>& wallsSensor, 
                                std::unique_ptr<ConcreteDirtSensor>& dirtSensor, 
                                std::unique_ptr<ConcreteBatteryMeter>& batteryMeter) {

    // Declare variables
    int numSteps = 0;
    int dirtLeft = initialDirtLevel;
    bool inDock = true;
    std::vector<std::tuple<int, int>> stepsHistory;
    stepsHistory.push_back(dockingStation);
    std::string status = "WORKING";
    std::tuple<int, int> currentPosition = dockingStation;
    std::string logPrefix = "[" + algorithmName + "," + houseName.substr(houseName.find_last_of("/\\") + 1) + "]";

    simulatorLogger.log(Logger::INFO, logPrefix + " Starting simulation.");
    if(maxBattery==1){
        status="FINISHED";
        inDock=true;
    }
    while (numSteps < maxSteps && maxBattery!=1) {

        Step next = algo.nextStep();  // Call the nextStep function of the algorithm
        simulatorLogger.log(Logger::INFO, logPrefix + " Num of Steps: " + std::to_string(numSteps));
        simulatorLogger.log(Logger::INFO, logPrefix + " Next step: " + std::to_string(static_cast<int>(next)));

        // Determine the new position based on the step
        int x = std::get<0>(currentPosition);
        int y = std::get<1>(currentPosition);

        switch (next) {
            case Step::North: 
                if (y > 0 && houseCopy[x][y - 1] != 'W') y -= 1; 
                else { status = "DEAD"; break; }
                break;
            case Step::East: 
                if (x < rows - 1 && houseCopy[x + 1][y] != 'W') x += 1; 
                else { status = "DEAD"; break; }
                break;
            case Step::South: 
                if (y < cols - 1 && houseCopy[x][y + 1] != 'W') y += 1; 
                else { status = "DEAD"; break; }
                break;
            case Step::West: 
                if (x > 0 && houseCopy[x - 1][y] != 'W') x -= 1; 
                else { status = "DEAD"; break; }
                break;
            case Step::Finish:
                status = "FINISHED";
                simulatorLogger.log(Logger::INFO, logPrefix + " Algorithm reported Finish, exiting loop");
                break;
            case Step::Stay:
                simulatorLogger.log(Logger::INFO, logPrefix + " Algorithm decided to Stay at (" + std::to_string(x) + ", " + std::to_string(y) + ")");
                break;
        }

        if (status == "FINISHED" || status == "DEAD") break;

        currentPosition = std::make_tuple(x, y);
        stepsHistory.push_back(currentPosition);  // Log the current position after each move
        simulatorLogger.log(Logger::INFO, logPrefix + " Moved to position: (" + std::to_string(x) + ", " + std::to_string(y) + ")");

        // Update the wall sensor with the new surroundings
        bool northWall = (y == 0 || houseCopy[x][y - 1] == 'W');
        bool eastWall = (x == rows - 1 || houseCopy[x + 1][y] == 'W');
        bool southWall = (y == cols - 1 || houseCopy[x][y + 1] == 'W');
        bool westWall = (x == 0 || houseCopy[x - 1][y] == 'W');
        wallsSensor->setWalls(northWall, eastWall, southWall, westWall);

        // Check if the robot is in the docking station
        inDock = (currentPosition == dockingStation);
        simulatorLogger.log(Logger::INFO, logPrefix + " In docking station: " + std::string(inDock ? "true" : "false"));

        // Decrease the dirt level at the current position if dirt level is between 1 and 9
        if (houseCopy[x][y] >= '1' && houseCopy[x][y] <= '9') {
            int dirt = houseCopy[x][y] - '0';
            if (next == Step::Stay) {
                dirt--;
                dirtLeft--;  
            }
            houseCopy[x][y] = (dirt > 0) ? ('0' + dirt) : '0';
            dirtSensor->setDirtLevel(dirt);
            simulatorLogger.log(Logger::INFO, logPrefix + " Cleaned dirt at position (" + std::to_string(x) + ", " + std::to_string(y) + "), new dirt level: " + std::to_string(dirt));
        } else {
            dirtSensor->setDirtLevel(0);
        }

        // Charge the battery if in the docking station and the robot stays
        if (inDock && next == Step::Stay) {
            int chargeAmount = std::max(1, static_cast<int>(maxBattery / 20));
            simulatorLogger.log(Logger::INFO, logPrefix + " Charge Amount is " + std::to_string(chargeAmount));
            batteryMeter->setBatteryState(std::min(batteryMeter->getBatteryState() + chargeAmount, maxBattery));
            simulatorLogger.log(Logger::INFO, logPrefix + " Battery charged to " + std::to_string(batteryMeter->getBatteryState()));
        } else {
            if (batteryMeter->getBatteryState() > 0) {
                batteryMeter->setBatteryState(batteryMeter->getBatteryState() - 1);
            }
            simulatorLogger.log(Logger::INFO, logPrefix + " Battery decreased to " + std::to_string(batteryMeter->getBatteryState()));
        }

        numSteps++;
    }

    // Handle the result after the loop
    if (status == "WORKING") {
        simulatorLogger.log(Logger::WARNING, "[" + algorithmName + "," + houseName + "] Algorithm ran out of steps");
    }

    int score = calculateScore(maxSteps, numSteps, dirtLeft, inDock, status);

    SimulationResult result = {houseName, algorithmName, numSteps, dirtLeft, inDock, status, score,stepsHistory};

    simulationResults.push_back(result);
    simulatorLogger.log(Logger::INFO, "[" + algorithmName + "," + houseName + "] Recorded result for house: " + houseName + " - Score: " + std::to_string(score));
    writeSimulationOutput(result);
    simulatorLogger.log(Logger::INFO, "[" + algorithmName + "," + houseName + "] Finished simulation.");
}













void MySimulator::generateSummaryCSV(const std::vector<SimulationResult>& results) {

    std::ofstream summaryFile("summary.csv");

    if (!summaryFile.is_open()) {

        std::cerr << "Failed to create summary.csv" << std::endl;

        return;

    }



    std::set<std::string> houseNames;

    std::set<std::string> algorithmNames;

    for (const auto& result : results) {

        houseNames.insert(result.houseName);

        algorithmNames.insert(result.algorithmName);

    }



    summaryFile << "Algorithm/House";

    for (const auto& houseName : houseNames) {

        summaryFile << "," << houseName;

    }

    summaryFile << "\n";



    for (const auto& algorithmName : algorithmNames) {

        summaryFile << algorithmName;

        for (const auto& houseName : houseNames) {

            auto it = std::find_if(results.begin(), results.end(), [&](const SimulationResult& result) {

                return result.algorithmName == algorithmName && result.houseName == houseName;

            });

            if (it != results.end()) {

                summaryFile << "," << it->score;

            } else {

                summaryFile << ",N/A";

            }

        }

        summaryFile << "\n";

    }



    summaryFile.close();

    std::cout << "Summary CSV generated successfully." << std::endl;

}

int MySimulator::calculateScore(int maxSteps, int numSteps, int dirtLeft, bool inDock, const std::string& status) {
    simulatorLogger.log(Logger::WARNING, "[" + std::to_string(maxSteps) + "] -maxSteps");
    simulatorLogger.log(Logger::WARNING, "[" + std::to_string(numSteps) + "] -numSteps");
    simulatorLogger.log(Logger::WARNING, "[" + std::to_string(dirtLeft) + "] -dirtLeft");
    simulatorLogger.log(Logger::WARNING, "[" + status + "] -status");
     

    if (status == "DEAD") {

        return maxSteps + dirtLeft * 300 + 2000;

    } else if (status == "FINISHED" && !inDock) {

        return maxSteps + dirtLeft * 300 + 3000;

    } else {

        return numSteps + dirtLeft * 300 + (inDock ? 0 : 1000);

    }

}

void MySimulator::writeStepsHistory(const std::string& filename, const std::vector<std::tuple<std::string, std::string, std::vector<std::tuple<int, int>>, std::tuple<int, int>, int>>& simulationResults) {

    std::ofstream outFile(filename);
    outFile << "[\n";

    for (size_t i = 0; i < simulationResults.size(); ++i) {
        const auto& [houseName, algoName, steps, dockingStation, score] = simulationResults[i];

        outFile << "  {\n";
        outFile << "    \"houseName\": \"" << houseName << "\",\n";
        outFile << "    \"algorithmName\": \"" << algoName << "\",\n";
        outFile << "    \"steps\": [\n";

        for (size_t j = 0; j < steps.size(); ++j) {
            const auto& step = steps[j];
            outFile << "      {\"x\": " << std::get<0>(step) << ", \"y\": " << std::get<1>(step) << "}";
            if (j < steps.size() - 1) {
                outFile << ",";
            }
            outFile << "\n";
        }

        outFile << "    ],\n";
        outFile << "    \"dockingStation\": {\"x\": " << std::get<0>(dockingStation) << ", \"y\": " << std::get<1>(dockingStation) << "},\n";
        outFile << "    \"score\": " << score << "\n";
        outFile << "  }";

        if (i < simulationResults.size() - 1) {
            outFile << ",";
        }

        outFile << "\n";
    }

    outFile << "]\n";
    outFile.close();
}



void MySimulator::writeHouseMatrix(const std::string& filename, const std::vector<std::tuple<std::string, std::vector<std::vector<char>>, int, int>>& houses) {
    std::ofstream outFile(filename);

    outFile << "{\n";
    outFile << "  \"houses\": [\n";

    for (size_t i = 0; i < houses.size(); ++i) {
        const auto& [houseName, houseMatrix, maxSteps, maxBattery] = houses[i];

        outFile << "    {\n";
        outFile << "      \"houseName\": \"" << houseName << "\",\n";
        outFile << "      \"maxSteps\": " << maxSteps << ",\n";
        outFile << "      \"maxBattery\": " << maxBattery << ",\n";
        outFile << "      \"house\": [\n";

        for (size_t row = 0; row < houseMatrix.size(); ++row) {
            outFile << "        [";
            for (size_t col = 0; col < houseMatrix[row].size(); ++col) {
                char cell = houseMatrix[row][col];
                int cellValue;

                if (cell == 'D' || cell == ' ') {
                    cellValue = 0;
                } else if (cell == 'W') {
                    cellValue = -1;
                } else if (isdigit(cell)) {
                    cellValue = cell - '0';
                } else {
                    cellValue = static_cast<int>(cell);  // Fallback to ASCII value if something unexpected is encountered
                }

                outFile << cellValue;
                if (col < houseMatrix[row].size() - 1) {
                    outFile << ", ";
                }
            }
            outFile << "]";
            if (row < houseMatrix.size() - 1) {
                outFile << ",\n";
            }
        }

        outFile << "\n      ]\n";
        outFile << "    }";
        if (i < houses.size() - 1) {
            outFile << ",";
        }
        outFile << "\n";
    }

    outFile << "  ]\n";
    outFile << "}\n";
    outFile.close();
}
void MySimulator::writeSimulationOutput(const SimulationResult& result) {
  // Construct the output file name based on the house and algorithm names
   std::string outputFileName = result.houseName + "-" + result.algorithmName + ".txt";
    
    // Open the file for writing
    std::ofstream outFile(outputFileName);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open " << outputFileName << " for writing." << std::endl;
        return;
    }

    // Write the required fields to the file
    outFile << "NumSteps = " << result.numSteps << "\n";
    outFile << "DirtLeft = " << result.dirtLeft << "\n";
    outFile << "Status = " << result.status << "\n";
    outFile << "InDock = " << (result.inDock ? "TRUE" : "FALSE") << "\n";
    outFile << "Score = " << result.score << "\n";
    
    // Write the steps sequence
    outFile << "Steps:\n";
    for (size_t i = 1; i < result.stepsHistory.size(); ++i) {
        char direction = calculateDirectionFromSteps(result.stepsHistory[i-1], result.stepsHistory[i]);
        outFile << direction;
    }
    if (result.status == "FINISHED") {
        outFile << 'F';
    }
    outFile << "\n";

    // Close the file
    outFile.close();
    std::cout << "Output file generated: " << outputFileName << std::endl;
}





char MySimulator::calculateDirectionFromSteps(const std::tuple<int, int>& previousPosition, const std::tuple<int, int>& currentPosition) {
    int prevX = std::get<0>(previousPosition);
    int prevY = std::get<1>(previousPosition);
    int currX = std::get<0>(currentPosition);
    int currY = std::get<1>(currentPosition);

    if (currX == prevX && currY == prevY) {
        return 's';  // Stay
    }
    else if (currX == prevX && currY < prevY) {
        return 'N';  // North
    }
    else if (currX > prevX && currY == prevY) {
        return 'E';  // East
    }
    else if (currX == prevX && currY > prevY) {
        return 'S';  // South
    }
    else if (currX < prevX && currY == prevY) {
        return 'W';  // West
    }
    
    return 'F';  // Finish or unknown case (should not happen under normal conditions)
}
