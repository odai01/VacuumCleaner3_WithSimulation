#ifndef CONCRETE_WALL_SENSOR_H
#define CONCRETE_WALL_SENSOR_H

#include "WallSensor.h"

class ConcreteWallSensor : public WallsSensor {
private:
    bool northWall;
    bool eastWall;
    bool southWall;
    bool westWall;

public:
    ConcreteWallSensor(bool north, bool east, bool south, bool west);

    bool isWall(Direction d) const override;
    void setWalls(bool north, bool east, bool south, bool west);
};

#endif // CONCRETE_WALL_SENSOR_H
