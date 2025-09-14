**View the video explanation/ presentation here** : 
[https://rmiteduau-my.sharepoint.com/:v:/g/personal/s3968971_student_rmit_edu_au/EfyBwbhsaUVHs-xnGF2V1KwB0J8gYWx7mAULSNQ0OKGpCQ?nav=eyJyZWZlcnJhbEluZm8iOnsicmVmZXJyYWxBcHAiOiJTdHJlYW1XZWJBcHAiLCJyZWZlcnJhbFZpZXciOiJTaGFyZURpYWxvZy1MaW5rIiwicmVmZXJyYWxBcHBQbGF0Zm9ybSI6IldlYiIsInJlZmVycmFsTW9kZSI6InZpZXcifX0%3D&e=aNl7sy](https://rmiteduau-my.sharepoint.com/:v:/g/personal/s3968971_student_rmit_edu_au/EfyBwbhsaUVHs-xnGF2V1KwB0J8gYWx7mAULSNQ0OKGpCQ?nav=eyJyZWZlcnJhbEluZm8iOnsicmVmZXJyYWxBcHAiOiJTdHJlYW1XZWJBcHAiLCJyZWZlcnJhbFZpZXciOiJTaGFyZURpYWxvZy1MaW5rIiwicmVmZXJyYWxBcHBQbGF0Zm9ybSI6IldlYiIsInJlZmVycmFsTW9kZSI6InZpZXcifX0%3D&e=eTjrVJ)

-Summary for Key Files
1. Maze.cpp & Maze.h:
Defines the Maze class, responsible for storing the maze’s base coordinates, dimensions (length and width), and mode (test or play). The class provides getter and setter functions to modify these attributes as needed.

2. mazeRunner.cpp:
Main file for the project, initializing parameters, handling user input through a menu system, and calling functions to generate, build, and solve mazes in Minecraft.
Supports modes such as -testmode and -shortestpath for testing or finding the shortest route.
Implements functions for generating maze structures, building them in Minecraft, and solving them using a player-controlled approach or automated shortest path.

3. menuUtils.h:
Provides utility functions for displaying various menus and team information within the command-line interface, enhancing interactivity and navigation for users.

4. Node.cpp & Node.h:
Defines the Node class, representing individual cells within the maze with attributes for coordinates, visitation status, and direction. This structure is essential for navigating and generating paths through the maze.

5. functions.h:
Contains helper functions for maze generation, structure manipulation, and solving. Key functions include:
initializeStructure and checkForObstacles for generating and scanning the maze.
buildStructure, deleteStructure, and flattenGround for building and removing mazes in Minecraft.
solveRightHand and shortestPath for player movement and pathfinding.

- BlackBox testing: 
1. mainMenuAndSolve Test:
This test checks if certain parts of the program should be executed in a specific order. For example:
You cannot build a maze without generating it first.
Similarly, the maze cannot be solved unless it has already been generated and created.

2. Tests 1-4:
These tests verify that non-integer inputs within loops are handled properly, preventing crashes or incorrect behavior.

3. Shortest Path Test:
This test evaluates the shortest path function and verifies its functionality when run with the command "./mazeRunner -testmode -shortestpath" in the terminal.

4. m3test:
This test checks the generate maze function, ensuring that the maze is initialized and structured correctly.

5. Obstacle Scan Test:
This test assesses a specific area in Minecraft where obstacles are present.
It ensures that the maze generation algorithm detects and builds around obstacles, which are marked as 'O' in the maze structure.

- FUNCTIONS FILE: created to take out most code from main menu and make it easier to read
- Structure of the maze: obstacles are marked as 'O', walls are marked as 'x', and spaces are marked as '.'
