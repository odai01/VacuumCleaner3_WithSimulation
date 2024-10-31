#include "ConcreteDirtSensor.h"

ConcreteDirtSensor::ConcreteDirtSensor(int initialDirtLevel) : currentDirtLevel(initialDirtLevel) {}

int ConcreteDirtSensor::dirtLevel() const {
    return currentDirtLevel;
}

void ConcreteDirtSensor::setDirtLevel(int dirtLevel) {
    currentDirtLevel = dirtLevel;
}

void ConcreteDirtSensor::decreaseDirt() {
    if (currentDirtLevel > 0) {
        currentDirtLevel--;
    }
}
