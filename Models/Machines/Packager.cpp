#include "Models/Machines/Packager.h"
#include "Models/Products/CPU.h"

void Packager::update(int tick) {
    if (getState() == BROKEN) {
        handleBrokenState();
        return;
    }

    if (getCurrentProduct() == nullptr) {
        fetchNextProduct();
        if (getCurrentProduct()==nullptr){
            return;
        }
    }

    if (getCurrentProduct() != nullptr) {
        decreaseRemainingTime(1);
        durability.decreaseHealth(1);
        if (!isRemainTime()) {
            Product* done = getCurrentProduct();
            setCurrentProduct(nullptr);
            done->setState(DELETED);
            setOutputNum(getOutputNum() + 1);
            if (durability.checkBreakdown()) {
                breakdown();
            }
        }
    }
}

std::string Packager::getInfo() const {
    return "Name: Packager, State: " + getStateName() + ", Queue: " + std::to_string(queue.getQueueSize()) + "/" + std::to_string(queue.getMaxQueueSize()) + ", Output: " + std::to_string(getOutputNum()) + ", Process Time: " + std::to_string(getProcessTime()) + ", Health: " + std::to_string(durability.getHealth()) + "%, Progress: " + std::to_string(getProgress()) + "%";
}

void Packager::switchCase(Case c) {
    // no-op
}
