#include "212609440_322776063_DFS.h"
#include "../AlgorithmRegistration.h"
#include "../../common/Logger.h"

Logger DFSAlgorithm::logger("dfs_algorithm.log");

DFSAlgorithm::DFSAlgorithm() : dockingStation(std::make_tuple(0, 0)), currentPosition(dockingStation) {
    logger.log(Logger::INFO, "DFSAlgorithm initialized");
}

void DFSAlgorithm::setDockingStation(int dockX, int dockY) {
    dockingStation = std::make_tuple(dockX, dockY);
    currentPosition = dockingStation;
    logger.log(Logger::INFO, "Docking station set to (" + std::to_string(dockX) + ", " + std::to_string(dockY) + ")");
}

void DFSAlgorithm::setMaxSteps(std::size_t maxSteps) {
    this->maxSteps = maxSteps;
    logger.log(Logger::INFO, "Max steps set to " + std::to_string(maxSteps));
}

void DFSAlgorithm::setWallsSensor(const WallsSensor& sensor) {
    this->wallsSensor = &sensor;
    logger.log(Logger::INFO, "Walls sensor set");
}

void DFSAlgorithm::setDirtSensor(const DirtSensor& sensor) {
    this->dirtSensor = &sensor;
    logger.log(Logger::INFO, "Dirt sensor set");
}

void DFSAlgorithm::setBatteryMeter(const BatteryMeter& meter) {
    this->batteryMeter = &meter;
    logger.log(Logger::INFO, "Battery meter set");
}

Step DFSAlgorithm::nextStep() {
    if (dfsStack.empty()) {
        logger.log(Logger::INFO, "DFS exloring starts from docking station");
    }

    int x = std::get<0>(currentPosition);
    int y = std::get<1>(currentPosition);

    visited.insert(currentPosition);

    int currentDirtLevel = dirtSensor->dirtLevel();
    updateInternalMap(x, y, currentDirtLevel);
    logger.log(Logger::INFO, "Currently in position (" + std::to_string(x) + ", " + std::to_string(y) + "), dirt level: " + std::to_string(currentDirtLevel));


    // If battery is low or we're currently charging, return to docking
    if (isCharging || batteryMeter->getBatteryState() <= pathToDocking.size()) {
        if (!isCharging) {
            isCharging=true;
            chargeStepsRemaining=20;
        }
        
        // Continue charging if necessary
        if (isCharging) {
            if(pathToDocking.empty()==true){
                logger.log(Logger::INFO, "Charging at docking station, steps remaining: " + std::to_string(chargeStepsRemaining));
                chargeStepsRemaining--;
                if(chargeStepsRemaining==0){
                    isCharging = false;
                    visited.clear();
                    while (!dfsStack.empty()) {dfsStack.pop();}
                    logger.log(Logger::INFO, "Last step of charging,then resuming DFS exploration");
                }
                return Step::Stay;
            }
            else{
                return calculateReturnPath();
            }
        }

        return calculateReturnPath();
    }

    if (currentDirtLevel > 0) {
        logger.log(Logger::INFO, "Staying to clean dirt at position (" + std::to_string(x) + ", " + std::to_string(y) + ")");
        dfsStack.push(Step::Stay);
        return Step::Stay;
    }
    // Explore deeper in possible directions
    for (Direction d : {Direction::North, Direction::East, Direction::South, Direction::West}) {
        int newX = x;
        int newY = y;
        Step step;

        switch (d) {
            case Direction::North:
                newY -= 1;
                step = Step::North;
                break;
            case Direction::East:
                newX += 1;
                step = Step::East;
                break;
            case Direction::South:
                newY += 1;
                step = Step::South;
                break;
            case Direction::West:
                newX -= 1;
                step = Step::West;
                break;
        }

        if (visited.find(std::make_tuple(newX, newY)) == visited.end() && !wallsSensor->isWall(d)) {
            dfsStack.push(step);
            pathToDocking.push_back(step);  // Track the path
            currentPosition = std::make_tuple(newX, newY);
            logger.log(Logger::INFO, "Exploring new direction: " + std::to_string(static_cast<int>(d)) + " to (" + std::to_string(newX) + ", " + std::to_string(newY) + ")");
            return step;
        }
    }

    // If all paths are explored, return to docking station
    return calculateReturnPath();
}


void DFSAlgorithm::updateInternalMap(int x, int y, int dirtLevel) {
    internalMap[std::make_tuple(x, y)] = dirtLevel;
    logger.log(Logger::INFO, "Updated internal map at position (" + std::to_string(x) + ", " + std::to_string(y) + ") with dirt level " + std::to_string(dirtLevel));
}

Step DFSAlgorithm::calculateReturnPath() {
    if (!pathToDocking.empty()) {
        Step lastStep = pathToDocking.back();
        pathToDocking.pop_back();

        switch (lastStep) {
            case Step::North: currentPosition = std::make_tuple(std::get<0>(currentPosition), std::get<1>(currentPosition) + 1); return Step::South;
            case Step::East:  currentPosition = std::make_tuple(std::get<0>(currentPosition) - 1, std::get<1>(currentPosition)); return Step::West;
            case Step::South: currentPosition = std::make_tuple(std::get<0>(currentPosition), std::get<1>(currentPosition) - 1); return Step::North;
            case Step::West:  currentPosition = std::make_tuple(std::get<0>(currentPosition) + 1, std::get<1>(currentPosition)); return Step::East;
            default: return Step::Stay;
        }
    }

    logger.log(Logger::INFO, "Reached docking station after cleaning all what's reachable, finishing simulation");
    return Step::Finish;
}

REGISTER_ALGORITHM(DFSAlgorithm);
