#include "Models/Machines/IonImplantator.h"
#include "Models/Products/DopedWafer.h"
#include <cstdlib>

void IonImplantator::update(int tick) {
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
            if (nextMachine && nextMachine->getQueueSize() < nextMachine->getMaxQueueSize()) {
                Product* done = getCurrentProduct();
                Product* processed = new DopedWafer(*done);
                done->setState(DELETED);

                nextMachine->addQueue(processed);
                setCurrentProduct(nullptr);
                setOutputNum(getOutputNum() + 1);
                if ((std::rand() / static_cast<float>(RAND_MAX)) < getBreakdownChance() || getHealth() <= 0) {
                    setHealth(getHealth() - 20);
                    setRemainingTime(getRepairTime());
                    breakdown();
                }
            } else {
                getCurrentProduct()->setState(DAMAGED);
                setCurrentProduct(nullptr);
                setState(IDLE);
            }
        }
    }
}

std::string IonImplantator::getInfo() const {
    return "Name: IonImplantator, State: " + getStateName() + ", Queue: " + std::to_string(getQueueSize()) + "/" + std::to_string(getMaxQueueSize()) + ", Output: " + std::to_string(getOutputNum()) + ", Process Time: " + std::to_string(getProcessTime()) + ", Health: " + std::to_string(getHealth()) + "%, Progress: " + std::to_string(getProgress()) + "%";
}

void IonImplantator::switchCase(Case c) {
    // no-op
}

void IonImplantator::setNextMachine(Machine* next) {
    nextMachine = next;
}