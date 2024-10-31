#ifndef CONCRETE_DIRT_SENSOR_H
#define CONCRETE_DIRT_SENSOR_H

#include "DirtSensor.h"

class ConcreteDirtSensor : public DirtSensor {
private:
    int currentDirtLevel;

public:
    ConcreteDirtSensor(int initialDirtLevel);

    int dirtLevel() const override;
    void setDirtLevel(int dirtLevel);

    void decreaseDirt();
};

#endif // CONCRETE_DIRT_SENSOR_H
