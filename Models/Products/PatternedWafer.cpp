#include "Models/Products/PatternedWafer.h"

PatternedWafer::PatternedWafer() : Product(PATTERNED) {}

PatternedWafer::PatternedWafer(const Product& other) {
    if (other.getType() != RAW) {
        //TODO: 파손 코드
    }
    type = PATTERNED;
}

std::string PatternedWafer::getTypeName() const {
    return "PatternedWafer";
}
