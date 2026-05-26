#include "Models/Machines/Packager.h"
#include "Models/Products/CPU.h"
#include <cstdlib>

void Packager::update(int tick) {
    if (getState() == BROKEN) {
        setRemainingTime(getRemainingTime() - 1);
        if (getRemainingTime() <= 0) repair();
        return;
    }

    if (getCurrentProduct() == nullptr) {
        setCurrentProduct(popQueue());
        if (getCurrentProduct()) {
            setState(PROCESSING);
            setRemainingTime(getProcessTime());
        } else {
            setState(IDLE);
            return;
        }
    }

    if (getCurrentProduct() != nullptr) {
        setRemainingTime(getRemainingTime() - 1);
        setHealth(getHealth() - 1);
        if (getRemainingTime() <= 0) {
            Product* done = getCurrentProduct();
            done->setState(DELETED);
            setCurrentProduct(nullptr);
            setOutputNum(getOutputNum() + 1);
            if ((std::rand() / static_cast<float>(RAND_MAX)) < getBreakdownChance() || getHealth() <= 0) {
                setHealth(getHealth() - 20);
                setRemainingTime(getRepairTime());
                breakdown();
            }
        }
    }
}

std::string Packager::getInfo() const {
    return "Name: Packager, State: " + getStateName() + ", Queue: " + std::to_string(getQueueSize()) + "/" + std::to_string(getMaxQueueSize()) + ", Output: " + std::to_string(getOutputNum()) + ", Process Time: " + std::to_string(getProcessTime()) + ", Health: " + std::to_string(getHealth()) + "%, Progress: " + std::to_string(getProgress()) + "%";
}

void Packager::switchCase(Case c) {
    // no-op
}
