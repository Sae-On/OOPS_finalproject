#include "Models/Products/DopedWafer.h"

DopedWafer::DopedWafer() : Product(DOPED) {}

DopedWafer::DopedWafer(const Product& other) {
    if (other.getType() != ETCHED) {
        //TODO: 파손 코드
    }
    type = DOPED;
}

std::string DopedWafer::getTypeName() const {
    return "DopedWafer";
}
