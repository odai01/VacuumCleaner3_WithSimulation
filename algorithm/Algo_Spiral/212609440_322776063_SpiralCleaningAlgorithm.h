#ifndef SPIRAL_CLEANING_ALGORITHM_H_
#define SPIRAL_CLEANING_ALGORITHM_H_

#include "../../common/AbstractAlgorithm.h"
#include "../../common/WallSensor.h"
#include "../../common/DirtSensor.h"
#include "../../common/BatteryMeter.h"
#include "../../common/Logger.h"
#include <set>
#include <tuple>
#include <vector>
#include <map>

class SpiralCleaningAlgorithm : public AbstractAlgorithm {
private:
    std::set<std::tuple<int, int>> visited;
    std::vector<Step> pathToDocking;
    std::map<std::tuple<int, int>, int> internalMap;
    const DirtSensor* dirtSensor = nullptr;
    const WallsSensor* wallsSensor = nullptr;
    const BatteryMeter* batteryMeter = nullptr;
    std::size_t maxSteps;
    std::tuple<int, int> dockingStation;
    std::tuple<int, int> currentPosition;
    bool isCharging = false;
    int chargeStepsRemaining = 20;  // Number of steps to stay in the docking station for charging

    static Logger logger;

public:
    SpiralCleaningAlgorithm();
    void setDockingStation(int dockX, int dockY);  // Removed override
    void setMaxSteps(std::size_t maxSteps) override;
    void setWallsSensor(const WallsSensor& sensor) override;
    void setDirtSensor(const DirtSensor& sensor) override;
    void setBatteryMeter(const BatteryMeter& meter) override;
    Step nextStep() override;

private:
    void updateInternalMap(int x, int y, int dirtLevel);
    Step calculateReturnPath();
};

#endif  // SPIRAL_CLEANING_ALGORITHM_H_
