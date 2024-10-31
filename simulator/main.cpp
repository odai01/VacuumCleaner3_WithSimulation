#include "mySimulator.h"
#include <iostream>

int main(int argc, char** argv) {
    try {
        MySimulator simulator;
        simulator.run(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred." << std::endl;
        return 1;
    }
    return 0;
}
