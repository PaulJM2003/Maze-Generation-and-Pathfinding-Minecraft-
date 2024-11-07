#include "Maze.h"
#include <random>


Maze::Maze(mcpp::Coordinate basePoint, unsigned int xlen, unsigned int zlen, bool mode)
{
    this->basePoint = basePoint;
    this->xlen = xlen;
    this->zlen = zlen;
    this->mode = mode;
}

Maze::~Maze()
{
}

mcpp::Coordinate Maze::getBasePoint() {
    return this->basePoint;
}
int Maze::getXLength() {
    return this->xlen;
}
int Maze::getZLength() {
    return this->zlen;
}
bool Maze::getMode() {
    return this->mode;
}

void Maze::setBasePoint(mcpp::Coordinate basePoint) {
    this->basePoint = basePoint;
}
void Maze::setXLength(int xlen) {
    this->xlen = xlen;
}
void Maze::setZLength(int zlen) {
    this->zlen = zlen;
}
void Maze::setMode(bool mode) {
    this->mode = mode;
}