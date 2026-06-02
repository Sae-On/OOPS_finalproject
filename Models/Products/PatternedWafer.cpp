#include "PatternedWafer.h"

PatternedWafer::PatternedWafer() : Product(PATTERNED) {}

PatternedWafer::PatternedWafer(const Product& other) : Product(other) {
    if (other.getType() != RAW) {
        state=DAMAGED;
    }
    type = PATTERNED;
}

std::string PatternedWafer::getTypeName() const {
    return "PatternedWafer";
}
