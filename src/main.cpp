#include "warehouse_simulator.h"

int main() {
    try {
        WarehouseSimulator simulator;
        simulator.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
