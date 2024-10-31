#include "ConcreteWallSensor.h"

ConcreteWallSensor::ConcreteWallSensor(bool north, bool east, bool south, bool west)
    : northWall(north), eastWall(east), southWall(south), westWall(west) {}

bool ConcreteWallSensor::isWall(Direction d) const {
    switch(d) {
        case Direction::North: return northWall;
        case Direction::East:  return eastWall;
        case Direction::South: return southWall;
        case Direction::West:  return westWall;
        default: return false;
    }
}

void ConcreteWallSensor::setWalls(bool north, bool east, bool south, bool west) {
    northWall = north;
    eastWall = east;
    southWall = south;
    westWall = west;
}
