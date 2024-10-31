#ifndef CONCRETE_BATTERY_METER_H
#define CONCRETE_BATTERY_METER_H

#include "BatteryMeter.h"

class ConcreteBatteryMeter : public BatteryMeter {
private:
    std::size_t batteryState;

public:
    ConcreteBatteryMeter(std::size_t initialBatteryState);

    std::size_t getBatteryState() const override;
    void setBatteryState(std::size_t newBatteryState);

    void chargeBattery(std::size_t chargeAmount);
};

#endif // CONCRETE_BATTERY_METER_H
