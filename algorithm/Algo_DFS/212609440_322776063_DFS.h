#ifndef DFS_ALGORITHM_H
#define DFS_ALGORITHM_H

#include "../../common/AbstractAlgorithm.h"
#include "../../common/WallSensor.h"
#include "../../common/DirtSensor.h"
#include "../../common/BatteryMeter.h"
#include "../../common/Logger.h"
#include <stack>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <tuple>

// Custom hash function for std::tuple<int, int>
namespace std {
    template <>
    struct hash<std::tuple<int, int>> {
        std::size_t operator()(const std::tuple<int, int>& t) const {
            auto h1 = std::hash<int>{}(std::get<0>(t));
            auto h2 = std::hash<int>{}(std::get<1>(t));
            return h1 ^ (h2 << 1); // or use boost::hash_combine
        }
    };
}

class DFSAlgorithm : public AbstractAlgorithm {
public:
    DFSAlgorithm();

    void setDockingStation(int dockX, int dockY) ;
    void setMaxSteps(std::size_t maxSteps) override;
    void setWallsSensor(const WallsSensor& sensor) override;
    void setDirtSensor(const DirtSensor& sensor) override;
    void setBatteryMeter(const BatteryMeter& meter) override;

    Step nextStep() override;

private:
    void updateInternalMap(int x, int y, int dirtLevel);
    Step calculateReturnPath();

    std::stack<Step> dfsStack;
    std::vector<Step> pathToDocking;
    std::unordered_set<std::tuple<int, int>> visited;
    std::unordered_map<std::tuple<int, int>, int> internalMap;
    std::tuple<int, int> dockingStation;
    std::tuple<int, int> currentPosition;
    std::size_t maxSteps;
    const WallsSensor* wallsSensor;
    const DirtSensor* dirtSensor;
    const BatteryMeter* batteryMeter;
    bool isCharging = false;
    int chargeStepsRemaining = 20;  // Number of steps to stay in the docking station for charging

    static Logger logger;
};

#endif // DFS_ALGORITHM_H
