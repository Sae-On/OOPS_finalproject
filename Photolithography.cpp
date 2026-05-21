#include "Photolithography.h"
#include <random>
#include "PatternedWafer.h"

void Photolithography::update(int tick) {
    if (getState() == STATE_BROKEN) {
        setRemainingTime(getRemainingTime() - 1);
        if (getRemainingTime() <= 0) {
            repair();
        }
        return;
    }

    if (getCurrentProduct() == nullptr) {
        setCurrentProduct(popQueue());
        if (getCurrentProduct()) {
            setState(STATE_PROCESSING);
            setRemainingTime(getProcessTime());
        } else {
            setState(STATE_IDLE);
            return;
        }
    }

    if (getCurrentProduct() != nullptr) {
        setRemainingTime(getRemainingTime() - 1);
        setHealth(getHealth() - 1);
        if (getRemainingTime() <= 0) {
            Product* done = getCurrentProduct();
            Product* processedProduct = new PatternedWafer(*done);
            delete done;

            nextMachine->addQueue(processedProduct);
            setCurrentProduct(nullptr);
            setOutputNum(getOutputNum() + 1);
            if ((std::rand() / static_cast<float>(RAND_MAX)) < getBreakdownChance() or getHealth() <= 0) {
                setHealth(getHealth() - 20);
                setRemainingTime(getRepairTime());
                breakdown();
            }
        }
    }
    
}

std::string Photolithography::getInfo() const {
    return "Photolithography: " + std::to_string(getOutputNum()) + " products processed, Health: " + std::to_string(getHealth());
}

void Photolithography::switchCase(int caseNum) {
    if (caseNum == CASE_NORMAL) {
        setProcessTime(3);
    } else if (caseNum == CASE_BOTTLENECK) {
        setProcessTime(3);
    }
}