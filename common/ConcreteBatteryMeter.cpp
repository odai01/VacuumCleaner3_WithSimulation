#include "ConcreteBatteryMeter.h"

ConcreteBatteryMeter::ConcreteBatteryMeter(std::size_t initialBatteryState) : batteryState(initialBatteryState) {}

std::size_t ConcreteBatteryMeter::getBatteryState() const {
    return batteryState;
}

void ConcreteBatteryMeter::setBatteryState(std::size_t newBatteryState) {
    batteryState = newBatteryState;
}

void ConcreteBatteryMeter::chargeBattery(std::size_t chargeAmount) {
    batteryState += chargeAmount;
}
