#ifndef ASSIGN_NODE_H
#define ASSIGN_NODE_H

#include <vector>
#include <mcpp/mcpp.h>

class Node{
public :
    Node(int x=0, int y=0, bool isVisited=false);
    ~Node();
    int x;
    int y;
    bool isVisited;
    std::string direction;

};

#endif //ASSIGN_NODE_H
