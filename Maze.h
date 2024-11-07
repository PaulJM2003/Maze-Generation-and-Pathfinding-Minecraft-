#ifndef ASSIGN_MAZE_H
#define ASSIGN_MAZE_H

#include <vector>
#include <mcpp/mcpp.h>

class Maze
{

public:
    Maze(mcpp::Coordinate basePoint, unsigned int xlen, unsigned int zlen, bool mode);
    ~Maze();

    mcpp::Coordinate getBasePoint();
    int getXLength();
    int getZLength();
    bool getMode();

    void setBasePoint(mcpp::Coordinate basePoint);
    void setXLength(int xlen);
    void setZLength(int zlen);
    void setMode(bool mode);

private:
    /* data */
    mcpp::Coordinate basePoint;
    unsigned int xlen;
    unsigned int zlen;
    bool mode;

};




#endif //ASSIGN_MAZE_H
