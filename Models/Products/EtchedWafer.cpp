#include "Models/Products/EtchedWafer.h"

EtchedWafer::EtchedWafer() : Product(ETCHED) {}

EtchedWafer::EtchedWafer(const Product& other) {
    if (other.getType() != PATTERNED) {
        //TODO: 파손 코드
    }
    type = ETCHED;
}

std::string EtchedWafer::getTypeName() const {
    return "EtchedWafer";
}
