#include "Models/Products/CPU.h"

CPU::CPU() : Product(CPU_TYPE) {}

CPU::CPU(const Product& other) {
    if (other.getType() != DOPED) {
        //TODO: 파손 코드
    }
    type = CPU_TYPE;
}

std::string CPU::getTypeName() const {
    return "CPU";
}
