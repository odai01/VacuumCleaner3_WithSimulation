# Assignment 3 - Advanced Vacuum Cleaner Simulation

## Contributors
- **Odai Agbaria**: 212609440
- **Mohammad Mahamid**: 322776063

## Overview
This project extends the vacuum cleaner simulation by implementing two advanced algorithms: **DFSAlgorithm** (Depth-First Search) and **SpiralCleaningAlgorithm**. These algorithms are designed to efficiently clean a house while optimizing the score based on the provided evaluation equation. Both algorithms ensure that the vacuum cleaner does not get "dead" (i.e., unable to return to the docking station) by considering the return path during their navigation, and when the battery is getting 
close to the length of the path to docking we return them.

Additionally, the project features a robust logging system and a Pygame-based visualization to animate the cleaning process and present the results.

## Approach
The project is divided into several key components:

### Algorithms
1. **DFSAlgorithm Class**: This algorithm uses Depth-First Search (DFS) to explore and clean the house. It is designed to ensure that the vacuum cleaner can always return to the docking station, thus optimizing the score according to the assignment's scoring formula.

2. **SpiralCleaningAlgorithm Class**: This algorithm follows a spiral pattern for cleaning. Like the DFS algorithm, it keeps track of the return path to the docking station, ensuring the vacuum cleaner can return safely and efficiently.

### Sensors and Components
- **ConcreteWallSensor**: Detects the presence of walls in the house.
- **ConcreteDirtSensor**: Monitors and detects dirt levels in each cell of the house.
- **ConcreteBatteryMeter**: Tracks and manages the battery state of the vacuum cleaner.

### Input Parsing
- The `MySimulator` class reads the house layout, docking station location, maximum battery capacity, and maximum iterations from an input file.

### Output Generation
- The `MySimulator` class generates a summary CSV file containing the scores for each algorithm and house combination, as well as detailed simulation logs.

### Logger
- The project includes a detailed logging system that captures runtime events, actions taken by the algorithms, and other relevant information for debugging and analysis.
we output three logging files, one from each algorithm, and the third is from the 
simulator, most comfortable is the one from the simulator, since the coordiantes 
are compatible with the houses, but in the other the coordinates are according to the internal map of the house in each algo.
### Visualization
- A Pygame-based visualization animates the cleaning process, displaying the vacuum's movement, dirt levels, and battery status. The visualization also includes features to show the algorithm's score and determine the winner of the cleaning competition.

## Input and Output Format

### Input Format
The input file should have the following structure:
1. **Line 1**: House name/description.
2. **Line 2**: Maximum steps for the simulation, formatted as `MaxSteps = <NUM>`.
3. **Line 3**: Maximum battery capacity, formatted as `MaxBattery = <NUM>`.
4. **Line 4**: Number of rows in the house, formatted as `Rows = <NUM>`.
5. **Line 5**: Number of columns in the house, formatted as `Cols = <NUM>`.
6. **Lines 6 and on**: The house matrix:
   - `W` represents a wall.
   - `0` represents an empty (clean) space.
   - Positive integers (1-9) represent dirt levels.
   - `D` represents the docking station.

### Output Format
The simulation generates a summary CSV file with the scores for each algorithm and house combination. The output format is as follows:

- **NumSteps**: Total number of steps performed.
- **DirtLeft**: Amount of dirt left in the house.
- **Status**: Vacuum cleaner status (`FINISHED/WORKING/DEAD`).
- **InDock**: Whether the vacuum cleaner is in the docking station when the simulation ends (`TRUE/FALSE`).
- **Score**: The final score calculated based on the formula provided in the assignment instructions.

## How to Build and Run

### Building the Project
1. **Configuring the Project**: cmake -S . -B ./build
2. **Building the Project**: cmake --build ./build
3. **Running the Project**: run ./build/simulator -house_path=./houses -algo_path=./    build/algorithm 
    this will produce three logging files, and two JSON files, and one csv file
4. **Running the Visualization**: Then, run: `python3 visualize.py`
    (Ensure you have Python and Pygame installed on your system)

## Logger
The project includes a robust logging mechanism that captures detailed runtime information for debugging and analysis. The log files, records various events and states such as battery level, current position, distance from the docking station, and overall steps. Additionally, it logs specific actions like moving towards the docking station, charging, cleaning, and finishing the simulation.

## Visualization:
The Pygame-based visualization animates the cleaning process, displaying both algorithms running simultaneously in a split-screen view of the house. Each half of the screen shows the progress of one algorithm, including movements, dirt levels, and battery status in real-time. You can press the "Continue" button to move to the next house and watch both algorithms run on it, making it easy to compare their performance visually.


## There is also a bonus file, there we will explain more about the logger and the visualization

## Hope you enjoy the simulation and the logger file
