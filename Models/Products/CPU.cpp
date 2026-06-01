#include "Models/Products/CPU.h"

CPU::CPU() : Product(CPU_TYPE) {}

CPU::CPU(const Product& other) : Product(other) {
    if (other.getType() != DOPED) {
        state=DAMAGED;
    } else {
        state = COMPLETED;
    }
    type = CPU_TYPE;
    
}

std::string CPU::getTypeName() const {
    return "CPU";
}
