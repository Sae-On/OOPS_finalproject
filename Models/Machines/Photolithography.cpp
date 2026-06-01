#include "Models/Machines/Photolithography.h"
#include "Models/Products/PatternedWafer.h"

void Photolithography::update(int tick) {
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
                Product* processedProduct = generateProduct(new PatternedWafer(*done));
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

std::string Photolithography::getInfo() const {
    return "Name: Photolithography, State: " + getStateName() + ", Queue: " + std::to_string(queue.getQueueSize()) + "/" + std::to_string(queue.getMaxQueueSize()) + ", Output: " + std::to_string(getOutputNum()) + ", Process Time: " + std::to_string(getProcessTime()) + ", Health: " + std::to_string(durability.getHealth()) + "%, Progress: " + std::to_string(getProgress()) + "%";
}

void Photolithography::switchCase(Case c) {
    // no-op
}

void Photolithography::setNextMachine(Machine* next) {
    nextMachine = next;
}