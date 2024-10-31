#include "212609440_322776063_SpiralCleaningAlgorithm.h"
#include "../AlgorithmRegistration.h"
#include "../../common/Logger.h"

Logger SpiralCleaningAlgorithm::logger("spiral_algorithm.log");

SpiralCleaningAlgorithm::SpiralCleaningAlgorithm() : dockingStation(std::make_tuple(0, 0)), currentPosition(dockingStation) {
    logger.log(Logger::INFO, "SpiralCleaningAlgorithm initialized");
}

void SpiralCleaningAlgorithm::setDockingStation(int dockX, int dockY) {
    dockingStation = std::make_tuple(dockX, dockY);
    currentPosition = dockingStation;
    logger.log(Logger::INFO, "Docking station set to (" + std::to_string(dockX) + ", " + std::to_string(dockY) + ")");
}

void SpiralCleaningAlgorithm::setMaxSteps(std::size_t maxSteps) {
    this->maxSteps = maxSteps;
    logger.log(Logger::INFO, "Max steps set to " + std::to_string(maxSteps));
}

void SpiralCleaningAlgorithm::setWallsSensor(const WallsSensor& sensor) {
    this->wallsSensor = &sensor;
    logger.log(Logger::INFO, "Walls sensor set");
}

void SpiralCleaningAlgorithm::setDirtSensor(const DirtSensor& sensor) {
    this->dirtSensor = &sensor;
    logger.log(Logger::INFO, "Dirt sensor set");
}

void SpiralCleaningAlgorithm::setBatteryMeter(const BatteryMeter& meter) {
    this->batteryMeter = &meter;
    logger.log(Logger::INFO, "Battery meter set");
}

Step SpiralCleaningAlgorithm::nextStep() {
    int x = std::get<0>(currentPosition);
    int y = std::get<1>(currentPosition);

    visited.insert(currentPosition);

    int currentDirtLevel = dirtSensor->dirtLevel();
    updateInternalMap(x, y, currentDirtLevel);
    logger.log(Logger::INFO, "Currently in position (" + std::to_string(x) + ", " + std::to_string(y) + "), dirt level: " + std::to_string(currentDirtLevel));

    // If battery is low or we're currently charging, return to docking
    if (isCharging || batteryMeter->getBatteryState() <= pathToDocking.size()) {
        if (!isCharging) {
            isCharging = true;
            chargeStepsRemaining = 20;
        }

        // Continue charging if necessary
        if (isCharging) {
            if (pathToDocking.empty()) {
                logger.log(Logger::INFO, "Charging at docking station, steps remaining: " + std::to_string(chargeStepsRemaining));
                chargeStepsRemaining--;
                if (chargeStepsRemaining == 0) {
                    isCharging = false;
                    visited.clear();
                    logger.log(Logger::INFO, "Finished charging, resuming spiral cleaning");
                }
                return Step::Stay;
            } else {
                return calculateReturnPath();
            }
        }

        return calculateReturnPath();
    }

    if (currentDirtLevel > 0) {
        logger.log(Logger::INFO, "Staying to clean dirt at position (" + std::to_string(x) + ", " + std::to_string(y) + ")");
        return Step::Stay;
    }

    // Follow the spiral pattern
    for (Direction d : {Direction::East, Direction::South, Direction::West, Direction::North}) {
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
            pathToDocking.push_back(step);  // Track the path
            currentPosition = std::make_tuple(newX, newY);
            logger.log(Logger::INFO, "Moving in spiral direction: " + std::to_string(static_cast<int>(d)) + " to (" + std::to_string(newX) + ", " + std::to_string(newY) + ")");
            return step;
        }
    }

    // If all paths are explored, return to docking station
    return calculateReturnPath();
}

void SpiralCleaningAlgorithm::updateInternalMap(int x, int y, int dirtLevel) {
    internalMap[std::make_tuple(x, y)] = dirtLevel;
    logger.log(Logger::INFO, "Updated internal map at position (" + std::to_string(x) + ", " + std::to_string(y) + ") with dirt level " + std::to_string(dirtLevel));
}

Step SpiralCleaningAlgorithm::calculateReturnPath() {
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

REGISTER_ALGORITHM(SpiralCleaningAlgorithm);
