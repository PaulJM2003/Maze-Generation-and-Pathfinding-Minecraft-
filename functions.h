#include <iostream>
#include <vector>
#include "Node.h"
#include <random>
#include <limits>
#include <mcpp/mcpp.h>
#include <unistd.h>
#include <queue>
#include "Agent.h"
#include <map>


// general purpose functions
// -----------------------------------------------


// read input parameters
// TODO: EDGE CASES CHECK IF INPUT ARE INTEGERS
void readParameters(int& length, int& width, mcpp::Coordinate& basePoint) {
    int basePointX, basePointY, basePointZ;
    std::cout << "Enter the basePoint of maze: " << std::endl;
    std::cin >> basePointX >> basePointY >> basePointZ;

 
    basePoint.x = basePointX;
    basePoint.y = basePointY;
    basePoint.z = basePointZ;

    std::cout << "Enter the length and width of maze: " << std::endl;
    std::cin >> length >> width;
    if (length <= 1) {
        length = 3;
    }
    if (width <= 1) {
        width =3;
    }
    if (length%2 != 1) {
        length = length + 1;
    }

    if (width%2 != 1) {
        width = width + 1;
    }
}


void printStructure(const int length, const int width, std::vector<std::vector<char>>& structure) {
    std::cout << "Structure: " << std::endl; 
    for (int i=0; i<length; i++) {
        for (int j = 0; j < width; j++) {
            std::cout << structure[i][j];
        }
        std::cout << std::endl;
    }
    std::cout << "**End Printing maze**" << std::endl;
}

void printInvalidChoiceAndClearBuffer(std::string menu) {
    if(menu == "mainloop") {
        std::cout << "Input Error: Enter a number between 1 and 5 ...." << std::endl;
    }
    else {
        std::cout << "Input Error: Enter a number between 1 and 3 ...." << std::endl;
    }
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}


// solve maze functions
// -----------------------------------------------
void randomSpot(Maze maze, std::vector<std::vector<char>> structure, Node& node, bool testMode) {
    mcpp::MinecraftConnection mc;
    //if testmode, put player in the farthest spot away from basepoint
    if(testMode) {
        mc.setPlayerPosition((maze.getBasePoint() + mcpp::Coordinate((maze.getXLength() - 2), 0, (maze.getZLength() - 2))));
    }
    else {
        std::vector<Node> spots;
        int numNodes = 0;
        for(int i=0; i<maze.getXLength(); i++) {
            for(int j=0; j<maze.getZLength(); j++) {
                if(structure[i][j] == '.') {
                    Node node(i,j,false);
                    spots.push_back(node);
                    numNodes++;
                }
            }
        }

        std::random_device engine;
        std::mt19937 gen(engine());
        std::uniform_int_distribution<int> size_dist(0, numNodes - 1);
        int spot = size_dist(engine);

        node.x = spots[spot].x;
        node.y = spots[spot].y;
    
        mc.setPlayerPosition(mcpp::Coordinate(node.x, 0, node.y) + maze.getBasePoint());
    }
}

void placeLime(mcpp::Coordinate newPosition , int& count) {
    mcpp::MinecraftConnection mc;
    mc.setBlock(newPosition, mcpp::Blocks::LIME_CARPET);
    sleep(1);
    mc.setBlock(newPosition, mcpp::Blocks::AIR);
    count++;
    std::cout << "step [" << count << "]: (" << newPosition.x << ", " << newPosition.y << ", " << newPosition.z << ")" << std::endl;
}

void solveRightHand(mcpp::Coordinate playerPosition, bool testMode, int& count, std::string nextDirection = "n") {
    mcpp::MinecraftConnection mc;    
    bool endReached = false;
    bool mazeSolved = false;
    mcpp::Coordinate newPlayerPosition = playerPosition;
    
    while(!endReached) {

        mcpp::Coordinate moveUp = playerPosition + mcpp::Coordinate(1, 0, 0); 
        mcpp::Coordinate moveDown = playerPosition + mcpp::Coordinate(-1, 0, 0); 
        mcpp::Coordinate moveLeft = playerPosition + mcpp::Coordinate(0, 0, -1); 
        mcpp::Coordinate moveRight = playerPosition + mcpp::Coordinate(0, 0, 1);
                        
        mcpp::BlockType blockTypeUp = mc.getBlock(moveUp);
        mcpp::BlockType blockTypeDown = mc.getBlock(moveDown);
        mcpp::BlockType blockTypeLeft = mc.getBlock(moveLeft);
        mcpp::BlockType blockTypeRight = mc.getBlock(moveRight);

        //special condition if brand new starting position AND TESTMODE
        if(nextDirection == "n" && testMode) {
            if(!(blockTypeRight == mcpp::Blocks::AIR) && (blockTypeUp == mcpp::Blocks::AIR)) {
                newPlayerPosition = moveUp;
                nextDirection = "Up";
            }
            
            else if(!(blockTypeDown == mcpp::Blocks::AIR) && blockTypeRight == mcpp::Blocks::AIR){
                newPlayerPosition  = moveRight;
                nextDirection = "Right";
            }
            else if(!(blockTypeUp == mcpp::Blocks::AIR) && blockTypeLeft == mcpp::Blocks::AIR) {
                newPlayerPosition = moveLeft;
                nextDirection = "Left";
            }
            else {
                newPlayerPosition = moveDown;
                nextDirection = "Down";
            }
            placeLime(newPlayerPosition, count);
        }
        
        //special condition if brand new startposition and NOT TESTMODE
        else if(nextDirection == "n" && !testMode) {
            
            std::vector<std::string> randDirections;
            int num = 0;

            if(!(blockTypeDown == mcpp::Blocks::AIR) && blockTypeRight == mcpp::Blocks::AIR) {
                randDirections.push_back("Right");
                num++;
            }
            if(!(blockTypeUp == mcpp::Blocks::AIR) && blockTypeLeft == mcpp::Blocks::AIR) {
                randDirections.push_back("Left");
                num++;
            }
            if(!(blockTypeRight == mcpp::Blocks::AIR) && blockTypeUp == mcpp::Blocks::AIR){
                randDirections.push_back("Up");
                num++;
            }
            if(!(blockTypeLeft == mcpp::Blocks::AIR) && blockTypeDown == mcpp::Blocks::AIR){
                randDirections.push_back("Down");
                num++;
            }

            std::random_device engine;
            std::mt19937 gen(engine());
            std::uniform_int_distribution<int> size_dist(0, num - 1);
            int index = size_dist(engine);

            nextDirection = randDirections[index];
            nextDirection == "Right" ? newPlayerPosition = moveRight : (
                nextDirection == "Up" ? newPlayerPosition = moveUp : (
                    nextDirection == "Left" ? newPlayerPosition = moveLeft : newPlayerPosition = moveDown));

            placeLime(newPlayerPosition, count);
        }

        //check if next direction == "Right"
        else if(nextDirection == "Right") {
            //check exitMaze condition
            if(!(blockTypeRight == mcpp::Blocks::ACACIA_WOOD_PLANK)
            && !(blockTypeUp == mcpp::Blocks::ACACIA_WOOD_PLANK)
            && !(blockTypeDown == mcpp::Blocks::ACACIA_WOOD_PLANK)
            && !(mc.getBlock(playerPosition + mcpp::Coordinate(1, 0, 1)) == mcpp::Blocks::ACACIA_WOOD_PLANK)
            && !(mc.getBlock(playerPosition + mcpp::Coordinate(-1, 0, 1)) == mcpp::Blocks::ACACIA_WOOD_PLANK)) {
                endReached = true;
                mazeSolved = true;
            }
            else if(blockTypeDown == mcpp::Blocks::AIR) {
                newPlayerPosition = moveDown;
                placeLime(newPlayerPosition , count);
                nextDirection = "Down";
               
            }
            else if(blockTypeRight == mcpp::Blocks::AIR) {
                newPlayerPosition = moveRight;
                placeLime(newPlayerPosition , count);
                
            }
            //if right is not air then the end is reached, now check whether to go up down or left
            else {
                endReached = true;
                if(blockTypeUp == mcpp::Blocks::AIR) {
                    nextDirection = "Up";
                }
                else {
                    nextDirection = "Left";
                }
            }
        }
        //check if next direction == "Up"
        else if(nextDirection == "Up") {

            //check exitMaze condition
            if(!(blockTypeUp == mcpp::Blocks::ACACIA_WOOD_PLANK)
            && !(blockTypeLeft == mcpp::Blocks::ACACIA_WOOD_PLANK)
            && !(blockTypeRight == mcpp::Blocks::ACACIA_WOOD_PLANK)
            && !(mc.getBlock(playerPosition + mcpp::Coordinate(1, 0, 1)) == mcpp::Blocks::ACACIA_WOOD_PLANK)
            && !(mc.getBlock(playerPosition + mcpp::Coordinate(1, 0, -1)) == mcpp::Blocks::ACACIA_WOOD_PLANK)) {

                endReached = true;
                mazeSolved = true;
            }

            else if(blockTypeRight == mcpp::Blocks::AIR) {
                newPlayerPosition = moveRight;
                placeLime(newPlayerPosition , count);
                nextDirection = "Right";
            }

            else if(blockTypeUp == mcpp::Blocks::AIR) {
                newPlayerPosition = moveUp;
                placeLime(newPlayerPosition , count);
            }
            //if up is not air then the end is reached, now check which direction to go
            else {
                endReached = true;
                if(blockTypeLeft == mcpp::Blocks::AIR) {
                    nextDirection = "Left";
                }
                else {
                    nextDirection = "Down";
                }
            }
        }
        //check if next direction == "Left"
        else if(nextDirection == "Left") {

            if(!(blockTypeLeft == mcpp::Blocks::ACACIA_WOOD_PLANK)
            && !(blockTypeUp == mcpp::Blocks::ACACIA_WOOD_PLANK)
            && !(blockTypeDown == mcpp::Blocks::ACACIA_WOOD_PLANK)
            && !(mc.getBlock(playerPosition + mcpp::Coordinate(1, 0, -1)) == mcpp::Blocks::ACACIA_WOOD_PLANK)
            && !(mc.getBlock(playerPosition + mcpp::Coordinate(-1, 0, -1)) == mcpp::Blocks::ACACIA_WOOD_PLANK)) {
                endReached = true;
                mazeSolved = true;
            }

            else if(blockTypeUp == mcpp::Blocks::AIR) {
                newPlayerPosition = moveUp;
                placeLime(newPlayerPosition , count);
                nextDirection = "Up";
            }
            
            else if(blockTypeLeft == mcpp::Blocks::AIR) {
                newPlayerPosition = moveLeft;
                placeLime(newPlayerPosition , count);
                
            }
            //if up is not air then the end is reached, now check which direction to go
            else {
                endReached = true;

                if(blockTypeUp == mcpp::Blocks::AIR) {
                    nextDirection = "Down";
                }
                else {
                    nextDirection = "Right";
                }
            }
        }

        //check if next direction == "Down"
        else if(nextDirection == "Down") {

            if(!(blockTypeDown == mcpp::Blocks::ACACIA_WOOD_PLANK)
            && !(blockTypeLeft == mcpp::Blocks::ACACIA_WOOD_PLANK)
            && !(blockTypeRight == mcpp::Blocks::ACACIA_WOOD_PLANK)
            && !(mc.getBlock(playerPosition + mcpp::Coordinate(-1, 0, 1)) == mcpp::Blocks::ACACIA_WOOD_PLANK)
            && !(mc.getBlock(playerPosition + mcpp::Coordinate(-1, 0, -1)) == mcpp::Blocks::ACACIA_WOOD_PLANK)) {
                endReached = true;

                mazeSolved = true;
            }

            else if(blockTypeLeft == mcpp::Blocks::AIR) {
                newPlayerPosition = moveLeft;
                placeLime(newPlayerPosition , count);
                nextDirection = "Left";
            }

            else if(blockTypeDown == mcpp::Blocks::AIR) {
                newPlayerPosition = moveDown;
                placeLime(newPlayerPosition , count);
                
            }
            //if up is not air then the end is reached, now check which direction to go
            else {
                endReached = true;
                if(blockTypeRight == mcpp::Blocks::AIR) {
                    nextDirection = "Right";
                }
                else {
                    nextDirection = "Up";
                }
            }
        }
        playerPosition = newPlayerPosition;
    }
    if(!mazeSolved) {
        solveRightHand(playerPosition, testMode, count , nextDirection);
    }
}





// build maze functions
// -----------------------------------------------
void deleteGround(Maze maze, std::vector<std::vector<char>> structure, std::vector<std::vector<int>> numGrassPlaced) {

    mcpp::MinecraftConnection mc;
    for(int i=0; i<maze.getXLength(); i++) {
        for(int j=0; j<maze.getZLength(); j++) {

            int y = numGrassPlaced[i][j];
            for (int k=1; k < (y + 1); k++) {
                usleep(10000);
                mc.setBlock(maze.getBasePoint() + mcpp::Coordinate(i, -k, j), mcpp::Blocks::AIR);
            }
        }
    }
}

void deleteStructure(Maze maze, std::vector<std::vector<char>> structure, std::vector<std::vector<int>> numGrassPlaced) {
    mcpp::MinecraftConnection mc;

    for(int i=0; i<maze.getXLength(); i++) {
        for(int j=0; j<maze.getZLength(); j++) {
            for (int k=0; k<3; k++) {

                if(mc.getBlock(maze.getBasePoint() + mcpp::Coordinate(i,k,j)) == mcpp::Blocks::ACACIA_WOOD_PLANK) {
                    mc.setBlock(maze.getBasePoint() + mcpp::Coordinate(i,k,j), mcpp::Blocks::AIR);
                }
            }
        }
    }
    deleteGround(maze, structure, numGrassPlaced);
}

void flattenGround(Maze maze, std::vector<std::vector<char>> structure, std::vector<std::vector<int>>& numGrassPlaced) {
    mcpp::MinecraftConnection mc;
    int numBlocks;

    for(int i=0; i<maze.getXLength(); i++) {
        std::vector<int> rowVal;
        for(int j=0; j<maze.getZLength(); j++) {
            numBlocks = 0;

            int y = -1;
            while((mc.getBlock(maze.getBasePoint() + mcpp::Coordinate(i,y,j))) == mcpp::Blocks::AIR) {
                mc.setBlock(maze.getBasePoint() + mcpp::Coordinate(i,y,j), mcpp::Blocks::GRASS);
                numBlocks++;
                y--;
            }
            rowVal.push_back(numBlocks);
        }
        numGrassPlaced.push_back(rowVal);
    }
}

void buildStructure(Maze maze, std::vector<std::vector<char>> structure, std::vector<std::vector<int>>& numGrassPlaced) {

    mcpp::MinecraftConnection mc;
    mc.setPlayerPosition(mcpp::Coordinate(0, 10, 0) + maze.getBasePoint());
    flattenGround(maze, structure, numGrassPlaced);

    for(int i=0; i<maze.getXLength(); i++) {
        for(int j=0; j<maze.getZLength(); j++) {
            if(structure[i][j] == 'x' || structure[i][j] == 'O') {
                // usleep(70000);
                //y = 0
                if(mc.getBlock(maze.getBasePoint() + mcpp::Coordinate(i, 0, j)) == mcpp::Blocks::AIR) {
                    mc.setBlock(maze.getBasePoint() + mcpp::Coordinate(i, 0, j), mcpp::Blocks::ACACIA_WOOD_PLANK);
                }

                //y = 1
                if (mc.getBlock(maze.getBasePoint() + mcpp::Coordinate(i, 1, j)) == mcpp::Blocks::AIR) {
                    mc.setBlock(maze.getBasePoint() + mcpp::Coordinate(i, 1, j), mcpp::Blocks::ACACIA_WOOD_PLANK);
                }
                
                //y = 2
                if(mc.getBlock(maze.getBasePoint() + mcpp::Coordinate(i, 2, j)) == mcpp::Blocks::AIR) {
                    mc.setBlock(maze.getBasePoint() + mcpp::Coordinate(i, 2, j), mcpp::Blocks::ACACIA_WOOD_PLANK);
                }
                
            }
        }
    }
}









//generate maze functions
// -----------------------------------------------

//initialize structure with holes every 2nd col 
void initializeStructure(Maze maze, std::vector<std::vector<char>>& structure, std::vector<Node>& startSpots, std::vector<Node>& allNodes) {
    for (int i=0; i<maze.getXLength(); i++) {
        std::vector<char> rowVals;
        
        for(int j=0; j<maze.getZLength(); j++) {
            //if row is odd, print x only
            if(i % 2 == 0) {
                rowVals.push_back('x');
            }
            else {
                //if col is odd print x, if even print ' '
                if(j % 2 == 0) {
                    rowVals.push_back('x');
                }
                else {
                    //mark 2nd outer walls to initialize available start spots
                    Node node(i,j,false);
                    rowVals.push_back(' ');
                    allNodes.push_back(node);
                    if((i == 1 || i == (maze.getXLength() - 2)) || (((j == 1 || j == maze.getZLength()- 2) && i % 2 == 1))) {
                        startSpots.push_back(node);
                    }
                }
            }
        }
        structure.push_back(rowVals);
    }
}


void checkForObstacles(Maze maze, std::vector<std::vector<char>>& structure, std::vector<Node> allNodes, std::vector<Node>& startSpots) {
    mcpp::MinecraftConnection mc;

    //check non nodes for obstacles
    for (int i=0; i<maze.getXLength(); i++) {
        for(int j=0; j<maze.getZLength(); j++) {
            if(!(mc.getBlock(maze.getBasePoint() + mcpp::Coordinate(i, 0, j)) == mcpp::Blocks::AIR) || !(mc.getBlock(maze.getBasePoint() + mcpp::Coordinate(i,1,j)) == mcpp::Blocks::AIR)) {
                structure[i][j] = 'O';
            }
        }
    }

    //check nodes for obstacles
    for (unsigned int i=0; i<allNodes.size(); i++) {
        if(!(mc.getBlock(maze.getBasePoint() + mcpp::Coordinate(allNodes[i].x, 0, allNodes[i].y)) == mcpp::Blocks::AIR) || !(mc.getBlock(maze.getBasePoint() + mcpp::Coordinate(allNodes[i].x, 1, allNodes[i].y)) == mcpp::Blocks::AIR)) {
            structure[allNodes[i].x][allNodes[i].y] = 'O';
            allNodes[i].isVisited = true;
        }
    }

    //update start spots for obstacles
    for (unsigned int i=0; i < startSpots.size(); i++) {
        if(structure[startSpots[i].x][startSpots[i].y] != '.') {
            startSpots.erase(startSpots.begin() + i);
        }
    }
}

void randomiseStartSpots(Maze maze, std::vector<Node>& startSpots, std::vector<std::vector<char>>& structure, Node& startNode) {
    int numSpots = startSpots.size();

    //rand generator, its like using srand(time(NULL)) but better
    //finds number of available start spots and randomises 0 - numspots
    std::random_device engine;
    std::mt19937 gen(engine());
    std::uniform_int_distribution<int> size_dist(0, numSpots - 1);
    int spot = size_dist(engine);

    int xval = startSpots[spot].x;
    int yval = startSpots[spot].y;

    structure[xval][yval] = '.';

    //finds closest edge to spot and opens up in that direction
    xval - 1 == 0 ? structure[xval - 1][yval] = '.' : (
        xval + 2 == maze.getXLength() ? structure[xval + 1][yval] = '.' : (
            yval - 1 == 0 ? structure[xval][yval - 1] = '.' : structure[xval][yval + 1] = '.'));
    
    //update starting node
    startNode.x = xval;
    startNode.y = yval;
    startNode.isVisited = true;
    
}

void updateNodeVisited(std::vector<Node>& allNodes, Node currPos) {
    for(unsigned int i=0; i<allNodes.size(); i++){
        if(allNodes[i].x == currPos.x && allNodes[i].y == currPos.y) {
            allNodes[i].isVisited = true;
        }
    }
}

void findNeighbors(std::vector<std::vector<char>> structure, Maze maze, std::vector<Node>& neighbors, Node& neighbor, int& numNeighbors, Node currPos) {
    mcpp::MinecraftConnection mc;

    //up
    if(currPos.x - 2 >= 0 && structure[currPos.x - 1][currPos.y] != 'O' && structure[currPos.x - 2][currPos.y] != 'O') {
        neighbor.x = currPos.x - 2;
        neighbor.y = currPos.y;
        neighbor.direction = "up";
        neighbors.push_back(neighbor);
        numNeighbors++;
    }
    //right
    if(currPos.y + 2 < maze.getZLength() && structure[currPos.x][currPos.y + 1] != 'O' && structure[currPos.x ][currPos.y + 2] != 'O') {
        neighbor.x = currPos.x;
        neighbor.y = currPos.y + 2;
        neighbor.direction = "right";
        neighbors.push_back(neighbor);
        numNeighbors++;
    }
    //down
    if(currPos.x + 2 < maze.getXLength() && structure[currPos.x + 1][currPos.y] != 'O' && structure[currPos.x + 2][currPos.y] != 'O') {
        neighbor.x = currPos.x + 2;
        neighbor.y = currPos.y;
        neighbor.direction = "down";
        neighbors.push_back(neighbor);
        numNeighbors++;
    }
    //left
    if(currPos.y - 2 >= 0 && structure[currPos.x][currPos.y - 1] != 'O' && structure[currPos.x][currPos.y - 2] != 'O') {
        neighbor.x = currPos.x;
        neighbor.y = currPos.y - 2;
        neighbor.direction = "left";
        neighbors.push_back(neighbor);
        numNeighbors++;
    }
    
}


void removeVisitedNeighbors(std::vector<Node>& neighbors, std::vector<Node> allNodes, int& numNeighbors) {
    for(long unsigned int i=0; i<neighbors.size(); i++) {
        for(long unsigned int j=0; j<allNodes.size(); j++) {
            if(((allNodes[j].x == neighbors[i].x) && (allNodes[j].y == neighbors[i].y)) && (allNodes[j].isVisited)) {
                // std::cout << "erased neighbor: " << neighbors[i].x << ", " << neighbors[i].y << std::endl;
                neighbors.erase(neighbors.begin() + i);
                numNeighbors--;
            }
        }
    }
}


void randomizeDirection(std::vector<Node>& neighbors, int& numNeighbors, std::vector<std::vector<char>>& structure, bool testMode) {
    Node temp;
    int r;

    if(testMode) {
        r = 0;
    }
    else {
        std::random_device engine;
        std::mt19937 gen(engine());
        std::uniform_int_distribution<int> size_dist(0, numNeighbors - 1);
        r = size_dist(engine);
    }

    int xNeighbor = neighbors[r].x;
    int yNeighbor = neighbors[r].y;

    neighbors[r].direction == "up" ? structure[xNeighbor + 1][yNeighbor] = '.' : (
        neighbors[r].direction == "down" ? structure[xNeighbor - 1][yNeighbor] = '.' : (
            neighbors[r].direction == "left" ? structure[xNeighbor][yNeighbor + 1] = '.' : structure[xNeighbor][yNeighbor - 1] = '.'));


    structure[xNeighbor][yNeighbor] = '.';
    temp = neighbors[r];
    neighbors.erase(neighbors.begin() + r);

    neighbors.push_back(temp);
}


// enhancement 2  functions
// -----------------------------------------------

bool checkExit(mcpp::Coordinate currentPos) {
    mcpp::MinecraftConnection mc;
    bool flag = false;

    mcpp::Coordinate moveUp = currentPos + mcpp::Coordinate(1, 0, 0); 
    mcpp::Coordinate moveDown = currentPos + mcpp::Coordinate(-1, 0, 0); 
    mcpp::Coordinate moveLeft = currentPos + mcpp::Coordinate(0, 0, -1); 
    mcpp::Coordinate moveRight = currentPos + mcpp::Coordinate(0, 0, 1);              
    mcpp::BlockType blockTypeUp = mc.getBlock(moveUp);
    mcpp::BlockType blockTypeDown = mc.getBlock(moveDown);
    mcpp::BlockType blockTypeLeft = mc.getBlock(moveLeft);
    mcpp::BlockType blockTypeRight = mc.getBlock(moveRight);


    if(!(blockTypeRight == mcpp::Blocks::ACACIA_WOOD_PLANK)
        && !(blockTypeUp == mcpp::Blocks::ACACIA_WOOD_PLANK)
        && !(blockTypeDown == mcpp::Blocks::ACACIA_WOOD_PLANK)
        && !(mc.getBlock(currentPos + mcpp::Coordinate(1, 0, 1)) == mcpp::Blocks::ACACIA_WOOD_PLANK)
        && !(mc.getBlock(currentPos + mcpp::Coordinate(-1, 0, 1)) == mcpp::Blocks::ACACIA_WOOD_PLANK)) {
            flag = true;
    }

    if(!(blockTypeUp == mcpp::Blocks::ACACIA_WOOD_PLANK)
        && !(blockTypeLeft == mcpp::Blocks::ACACIA_WOOD_PLANK)
        && !(blockTypeRight == mcpp::Blocks::ACACIA_WOOD_PLANK)
        && !(mc.getBlock(currentPos + mcpp::Coordinate(1, 0, 1)) == mcpp::Blocks::ACACIA_WOOD_PLANK)
        && !(mc.getBlock(currentPos + mcpp::Coordinate(1, 0, -1)) == mcpp::Blocks::ACACIA_WOOD_PLANK)) {
            flag = true;
    }

    if(!(blockTypeLeft == mcpp::Blocks::ACACIA_WOOD_PLANK)
        && !(blockTypeUp == mcpp::Blocks::ACACIA_WOOD_PLANK)
        && !(blockTypeDown == mcpp::Blocks::ACACIA_WOOD_PLANK)
        && !(mc.getBlock(currentPos + mcpp::Coordinate(1, 0, -1)) == mcpp::Blocks::ACACIA_WOOD_PLANK)
        && !(mc.getBlock(currentPos + mcpp::Coordinate(-1, 0, -1)) == mcpp::Blocks::ACACIA_WOOD_PLANK)) {
            flag = true;
    }


    if(!(blockTypeDown == mcpp::Blocks::ACACIA_WOOD_PLANK)
        && !(blockTypeLeft == mcpp::Blocks::ACACIA_WOOD_PLANK)
        && !(blockTypeRight == mcpp::Blocks::ACACIA_WOOD_PLANK)
        && !(mc.getBlock(currentPos + mcpp::Coordinate(-1, 0, 1)) == mcpp::Blocks::ACACIA_WOOD_PLANK)
        && !(mc.getBlock(currentPos + mcpp::Coordinate(-1, 0, -1)) == mcpp::Blocks::ACACIA_WOOD_PLANK)) {
            flag = true;
    }

    //check up first
    // if(!(((mc.getBlock(currentPos + mcpp::Coordinate(1, 0, 0)))) == mcpp::Blocks::ACACIA_WOOD_PLANK)
    //     //right
    //     && !(((mc.getBlock(currentPos + mcpp::Coordinate(0, 0, 1)))) == mcpp::Blocks::ACACIA_WOOD_PLANK)
    //     //left
    //     && !(((mc.getBlock(currentPos + mcpp::Coordinate(0, 0, -1)))) == mcpp::Blocks::ACACIA_WOOD_PLANK)
    //     //down
    //     && !(((mc.getBlock(currentPos + mcpp::Coordinate(-1, 0, 0)))) == mcpp::Blocks::ACACIA_WOOD_PLANK)
    //     && !(mc.getBlock(currentPos + mcpp::Coordinate(1, 0, -1)) == mcpp::Blocks::ACACIA_WOOD_PLANK)
    //     && !(mc.getBlock(currentPos + mcpp::Coordinate(1, 0, 1)) == mcpp::Blocks::ACACIA_WOOD_PLANK)
    //     && !(mc.getBlock(currentPos + mcpp::Coordinate(-1, 0, -1)) == mcpp::Blocks::ACACIA_WOOD_PLANK)
    //     && !(mc.getBlock(currentPos + mcpp::Coordinate(-1, 0, 1)) == mcpp::Blocks::ACACIA_WOOD_PLANK)) {
    //     flag = true;
    // }
    return flag;
}

void backTrace(int yvalue, std::pair<int, int> exitCoords, std::pair<int, int> startLocation, std::map<std::pair<int, int> , std::pair<int, int>> map, int& numStep) {

    bool exit = false;
    std::vector<mcpp::Coordinate> path;

    std::pair<int, int> currentLocation = exitCoords;
    std::pair<int, int> parentLocation;

    while (!exit) {
        parentLocation = map[currentLocation];
        mcpp::Coordinate coord(parentLocation.first, yvalue, parentLocation.second);
        path.insert(path.begin(), coord);
        currentLocation = parentLocation;
        if(currentLocation == startLocation) {
            exit = true;
        }
    }
    for(unsigned int i=0; i<path.size(); i++) {
        placeLime(path[i], numStep);
    }
}


void shortestPath(mcpp::Coordinate playerPosition, int& numStep) {

    mcpp::MinecraftConnection mc;
    std::queue<mcpp::Coordinate> queue;
    mcpp::Coordinate currentPos = playerPosition;
    std::pair<int, int> playerPositionPair = {playerPosition.x, playerPosition.z};
    std::vector<mcpp::Coordinate> visited;

    visited.push_back(currentPos);
    queue.push(currentPos);

    std::map<std::pair<int, int>, std::pair<int, int>> map;
    bool foundExit = false;

    while(!queue.empty() && !foundExit) {

        

        std::vector<mcpp::Coordinate> neighbors;
        currentPos = queue.front();
        queue.pop();

        

        mcpp::Coordinate moveUp = currentPos + mcpp::Coordinate(1, 0, 0); 
        mcpp::Coordinate moveDown = currentPos + mcpp::Coordinate(-1, 0, 0); 
        mcpp::Coordinate moveLeft = currentPos + mcpp::Coordinate(0, 0, -1); 
        mcpp::Coordinate moveRight = currentPos + mcpp::Coordinate(0, 0, 1);              
        mcpp::BlockType blockTypeUp = mc.getBlock(moveUp);
        mcpp::BlockType blockTypeDown = mc.getBlock(moveDown);
        mcpp::BlockType blockTypeLeft = mc.getBlock(moveLeft);
        mcpp::BlockType blockTypeRight = mc.getBlock(moveRight);

        //up    
        if(blockTypeUp == mcpp::Blocks::AIR) {
            // std::cout << "a" << std::endl;
            neighbors.push_back(moveUp);
        }
        //right
        if(blockTypeRight == mcpp::Blocks::AIR) {
            // std::cout << "b" << std::endl;
            neighbors.push_back(moveRight);
        }
        //down
        if(blockTypeDown == mcpp::Blocks::AIR) {
            // std::cout << "c" << std::endl;
            neighbors.push_back(moveDown);
        }
        //left
        if(blockTypeLeft == mcpp::Blocks::AIR) {
            // std::cout << "d" << std::endl;
            neighbors.push_back(moveLeft);
        }

        for (unsigned int i=0; i<visited.size(); i++) {
            for (unsigned int j=0; j<neighbors.size(); j++) {
                if(neighbors[j] == visited[i]) {
                    neighbors.erase(neighbors.begin() + j);
                }
            }
        }

        std::pair<int, int> currentPosPair = {currentPos.x, currentPos.z};
        for (unsigned int i=0; i<neighbors.size(); i++) {
            std::pair<int, int> neighborPair = {neighbors[i].x, neighbors[i].z};
            map[neighborPair] = currentPosPair;

            visited.push_back(neighbors[i]);
            queue.push(neighbors[i]);
        }

        mc.setBlock(currentPos, mcpp::Blocks::LIME_CARPET);
        

        if(checkExit(currentPos)) {
            for(unsigned int i=0; i<visited.size(); i++) {
                mc.setBlock(visited[i], mcpp::Blocks::AIR);
            }
            // std::cout << "end found" << std::endl;
            foundExit = true;
            numStep = 0;
            backTrace(playerPosition.y, currentPosPair, playerPositionPair, map, numStep);  
        }
    }
}