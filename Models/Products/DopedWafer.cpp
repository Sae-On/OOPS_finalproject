#include "Models/Products/DopedWafer.h"

DopedWafer::DopedWafer() : Product(DOPED) {}

DopedWafer::DopedWafer(const Product& other) : Product(other) {
    if (other.getType() != ETCHED) {
        state=DAMAGED;
    }
    type = DOPED;
}

std::string DopedWafer::getTypeName() const {
    return "DopedWafer";
}
