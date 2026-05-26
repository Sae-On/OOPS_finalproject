#include "Models/Machines/Etcher.h"
#include <random>
#include "Models/Products/EtchedWafer.h"

void Etcher::update(int tick) {
    if (getState() == BROKEN) {
        setRemainingTime(getRemainingTime() - 1);
        if (getRemainingTime() <= 0) {
            repair();
        }
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
                Product* processedProduct = new EtchedWafer(*done);
                done->setState(DELETED);

                nextMachine->addQueue(processedProduct);
                setCurrentProduct(nullptr);
                setOutputNum(getOutputNum() + 1);
                if ((std::rand() / static_cast<float>(RAND_MAX)) < getBreakdownChance() or getHealth() <= 0) {
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

std::string Etcher::getInfo() const {
    return "Name: Etcher, State: " + getStateName() + ", Queue: " + std::to_string(getQueueSize()) + "/" + std::to_string(getMaxQueueSize()) + ", Output: " + std::to_string(getOutputNum()) + ", Process Time: " + std::to_string(getProcessTime()) + ", Health: " + std::to_string(getHealth()) + "%, Progress: " + std::to_string(getProgress()) + "%";
}

void Etcher::switchCase(Case c) {
    if (c == NORMAL) {
        setProcessTime(4);
    } else if (c == BOTTLENECK) {
        setProcessTime(15);
    }
}

void Etcher::setNextMachine(Machine* next) {
    nextMachine = next;
}