#include "EtchedWafer.h"

EtchedWafer::EtchedWafer() : Product(ETCHED) {}

EtchedWafer::EtchedWafer(const Product& other) : Product(other) {
    if (other.getType() != PATTERNED) {
        state=DAMAGED;
    }
    type = ETCHED;
}

std::string EtchedWafer::getTypeName() const {
    return "EtchedWafer";
}
