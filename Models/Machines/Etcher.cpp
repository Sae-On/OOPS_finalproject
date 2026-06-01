#include "Models/Machines/Etcher.h"
#include "Models/Products/EtchedWafer.h"

void Etcher::update(int tick) {
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
            if (nextMachine && nextMachine->getQueue().getQueueSize() < nextMachine->getQueue().getMaxQueueSize()) {
                Product* done = getCurrentProduct();
                Product* processedProduct = generateProduct(new EtchedWafer(*done));
                setCurrentProduct(nullptr);
                done->setState(DELETED);

                nextMachine->getQueue().addQueue(processedProduct);
                
                setOutputNum(getOutputNum() + 1);
                if (durability.checkBreakdown()) {
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
    return "Name: Etcher, State: " + getStateName() + ", Queue: " + std::to_string(queue.getQueueSize()) + "/" + std::to_string(queue.getMaxQueueSize()) + ", Output: " + std::to_string(getOutputNum()) + ", Process Time: " + std::to_string(getProcessTime()) + ", Health: " + std::to_string(durability.getHealth()) + "%, Progress: " + std::to_string(getProgress()) + "%";
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
