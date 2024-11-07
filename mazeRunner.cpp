#include <iostream>

#include <mcpp/mcpp.h>

#include "menuUtils.h"
#include "Maze.h"
#include "Agent.h"
#include "Node.h"
#include <vector>
#include <cstdlib>
#include <stack>
#include "functions.h"


int main(int argc, char** argv){

    //check if there is -testmode being put, if it is set testmode to true
    bool testMode = false;
    bool solveShortestPath = false;
    if(argc == 2) {
        
        if(std::string(argv[1]) == "-testmode") {
            std::cout << "running in testing mode" << std::endl;
            testMode = true;
        }
        
        if(std::string(argv[1]) == "-shortestpath") {
            std::cout << "shortest path function activated" << std::endl;
            solveShortestPath = true;
        }
    }
    else if (argc == 3) {
        
        if((std::string(argv[1]) == "-testmode") && (std::string(argv[2]) == "-shortestpath")) {
            std::cout << "running in test mode and shortest path function activated" << std::endl;
            testMode = true;
            solveShortestPath = true;
        }
    }

    printStartText();
    mcpp::MinecraftConnection mc; 
    mc.doCommand("time set day"); 

    
    int length, width;
    mcpp::Coordinate basePoint;
    std::vector<std::vector<char>> structure, oldStructure;
    std::vector<std::vector<int>> numGrassPlaced, oldNumGrassPlaced;
    Maze maze(basePoint, length, width, testMode);
    Maze oldMaze(basePoint, length, width, testMode);
    std::string choice;
    bool exitMaze = false;
    bool playerInitialized = false;
    bool mazeBuilt = false;
    int numBuild = 0;
    int counter=0;
    

    while (!exitMaze) {

        printMainMenu();

        std::cin >> choice;

        //Generate Maze 
        if(choice == "1") {
            std::string choice1;
            bool back = false;
            printGenerateMazeMenu();
            while(!back) {

                std::cin >> choice1;
            
                // read maze from terminal
                if(choice1 == "1") {
                    structure.clear();

                    readParameters(length, width, basePoint);
                    maze.setBasePoint(basePoint);
                    maze.setXLength(length);
                    maze.setZLength(width);
                    counter++;

                    std::cout << "Enter The Maze structure: " << std::endl;

                    char charValue;
                    for (int i = 0; i < length; i++) {
                        std::vector<char> rowVal;
                        for(int j = 0; j < width; j++) {
                            std::cin >> charValue;
                            rowVal.push_back(charValue);
                        }
                        structure.push_back(rowVal);
                    }

                    std::cout << "Maze Read successfully" << std::endl;
                    std::cout << "**Printing Maze**" << std::endl;
                    std::cout << "BasePoint: (" << basePoint.x << ", " << basePoint.y << ", " << basePoint.z << ")" << std::endl;

                    printStructure(length, width, structure);

                    back = true;

                }
                // generate random maze
                else if(choice1 == "2") {
                    structure.clear();

                    readParameters(length, width, basePoint);
                    maze.setBasePoint(basePoint);
                    maze.setXLength(length);
                    maze.setZLength(width);
                    counter++;
                    //initialize 2D vector with the maze structure
                    std::vector<Node> startSpots, allNodes;
                    initializeStructure(maze, structure, startSpots, allNodes);
                    
                    checkForObstacles(maze, structure, allNodes, startSpots);

                    //if testmode, makes sure start point is (1,1) and gate is (0,0)
                    Node currPos;
                    if(testMode) {
                        currPos.x = 1;
                        currPos.y = 1;
                        currPos.isVisited = true;
                        structure[1][1] = '.';
                        structure[1][0] = '.';
                    } 
                    else {
                        //randomise start spots, also updates starting node
                        randomiseStartSpots(maze, startSpots, structure, currPos);
                    }

                    //start recursive search
                    std::stack<Node> stack;
                    stack.push(currPos);

                    int numNeighbors;
                    while(!stack.empty()) {

                        std::vector<Node> neighbors;
                        numNeighbors = 0;
                        currPos = stack.top();
                        stack.pop();

                        //make current node visited = true
                        updateNodeVisited(allNodes, currPos);

                        //finds neighbors of current node (up, down left, right) if exists, adds to neighbors vector and numneighbors++
                        Node neighbor;
                        findNeighbors(structure, maze, neighbors, neighbor, numNeighbors, currPos);
                        
                        //check if nodes in neighbor have already been visited, if so remove them 
                        removeVisitedNeighbors(neighbors, allNodes, numNeighbors);
                        removeVisitedNeighbors(neighbors, allNodes, numNeighbors);
                        
                        //randomize and pick a direction based on number of neighbors if > 0
                        if(numNeighbors > 0) {
                            randomizeDirection(neighbors, numNeighbors, structure, testMode);

                            //push all nodes in neighbors to stack
                            for(unsigned int i=0; i<neighbors.size(); i++) {
                                stack.push(neighbors[i]);
                            }
                        }

                        //activates if numNeighbors == 0 on the latest iteration and goes to next available node
                        if(!stack.empty()) {
                            Node newNode = stack.top();
                            if(numNeighbors == 0 && (structure[newNode.x][newNode.y] == ' ')) {
                                structure[newNode.x][newNode.y] = '.';

                                if(newNode.direction == "up" && structure[newNode.x + 1][newNode.y] == 'x') {
                                    structure[newNode.x + 1][newNode.y] = '.';
                                }
                                else if(newNode.direction == "down" && structure[newNode.x - 1][newNode.y] == 'x') {
                                    structure[newNode.x - 1][newNode.y] = '.';
                                }
                                else if(newNode.direction == "left" && structure[newNode.x][newNode.y + 1] == 'x') {
                                    structure[newNode.x][newNode.y + 1] = '.';
                                }
                                else if(newNode.direction == "right" && structure[newNode.x][newNode.y - 1] == 'x') {
                                    structure[newNode.x][newNode.y - 1] = '.';
                                }
                            }
                        }
                    }

                    std::cout << "Maze generated successfully" << std::endl;
                    std::cout << "**Printing Maze**" << std::endl;
                    std::cout << "BasePoint: (" << basePoint.x << ", " << basePoint.y << ", " << basePoint.z << ")" << std::endl;
                    printStructure(length, width, structure);

                    back = true;
                }
                // back
                else if(choice1 == "3") {
                    back = true;
                }
                 //error prevention
                else {
                    printSolveMazeMenu();
                    printInvalidChoiceAndClearBuffer("innerloop");
                    if(std::cin.eof()) {
                        back = true;
                        exitMaze = true;
                    }
                }
            }
        }

        //Build Maze in Minecrcaft
        else if(choice == "2") {
            if(counter!=0){
                mazeBuilt = true;
                numBuild++;
                
                if(numBuild <= 1) {
                    buildStructure(maze, structure, numGrassPlaced);
                }
                else {
                    deleteStructure(oldMaze, structure, numGrassPlaced);
                    numGrassPlaced.clear();
                    buildStructure(maze, structure, numGrassPlaced);
                }

                oldMaze = maze;
                oldStructure = structure;
                oldNumGrassPlaced = numGrassPlaced;
            }
            else{
                std::cout << "Error: Please generate a maze before building" << std::endl;
            }
        }

        //Solve Maze
        else if(choice == "3") {
            std::string choice3;
            bool back = false;
            printSolveMazeMenu();

            while(!back) {
                std::cin >> choice3;

                //solve manually
                if(choice3 == "1") {

                    if(!mazeBuilt) {
                        std::cout << "Error: Please build the maze before solving" << std::endl;
                    }
                    else {

                        Node node;
                        Maze maze(basePoint, length, width, true);
                        randomSpot(maze, structure, node, testMode);

                        playerInitialized = true;
                        std::cout << "Maze ready to Solve..." << std::endl;
                    }
                    
                    back = true;
                }
                
                //show escape route
                else if(choice3 == "2") {

                    if(!playerInitialized) {
                        std::cout << "Error: Initialize the player using Solve manually first." << std::endl;
                        back = true;
                    }
                    else {
                        mcpp::MinecraftConnection mc; 
                        mcpp::Coordinate playerPosition = mc.getPlayerPosition();
                        mcpp::Coordinate exitCoordinates;
                        int numStep = 0;
                        if(!solveShortestPath) {
                            solveRightHand(playerPosition, testMode, numStep);
                        }
                        else {
                            shortestPath(playerPosition, numStep);
                        }
                        back = true;
                    }
                }

                //back
                else if(choice3 == "3") {
                    back = true;
                }

                //error prevention
                else {
                    printSolveMazeMenu();
                    printInvalidChoiceAndClearBuffer("innerloop");
                    if(std::cin.eof()) {
                        back = true;
                        exitMaze = true;
                    }
                }
            }
        }

        //Print Team info
        else if(choice == "4") {
            printTeamInfo();
        }

        //exit maze
        else if(choice == "5") {
            if(numBuild != 0) {
                deleteStructure(oldMaze, structure, numGrassPlaced);
            }
            counter = 0;
            exitMaze = true;
        }

        else {
            printInvalidChoiceAndClearBuffer("mainloop");
            if(std::cin.eof()) {
                exitMaze = true;
            }
        }
    }

    printExitMassage();


    return EXIT_SUCCESS;

}