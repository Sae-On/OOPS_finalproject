#include "RawWafer.h"

RawWafer::RawWafer() : Product(RAW) {}

std::string RawWafer::getTypeName() const {
    return "RawWafer";
}
