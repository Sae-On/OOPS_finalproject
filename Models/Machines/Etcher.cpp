#include "Models/Machines/Etcher.h"
#include "Models/Products/EtchedWafer.h"
#include <cstdlib>

void Etcher::update(int tick) {
    if (getState() == MACHINE_BROKEN) {
        setRemainingTime(getRemainingTime() - 1);
        if (getRemainingTime() <= 0) {
            repair();
        }
        return;
    }

    if (getCurrentProduct() == nullptr) {
        setCurrentProduct(popQueue());
        if (getCurrentProduct()) {
            setState(MACHINE_PROCESSING);
            setRemainingTime(getProcessTime());
        } else {
            setState(MACHINE_IDLE);
            return;
        }
    }

    if (getCurrentProduct() != nullptr) {
        setRemainingTime(getRemainingTime() - 1);
        setHealth(getHealth() - 1);

        if (getRemainingTime() <= 0) {
            if (nextMachine && nextMachine->getQueueSize() < nextMachine->getMaxQueueSize()) {
                Product* done = getCurrentProduct();
                bool wasDamaged = (done->getState() == DAMAGED);
                Product* processed = new EtchedWafer(*done);
                delete done; // 메모리 누수 수정

                if (wasDamaged || (std::rand() / static_cast<float>(RAND_MAX)) < getBreakdownChance()) {
                    processed->setState(DAMAGED);
                }

                setCurrentProduct(nullptr);
                nextMachine->addQueue(processed);
                setOutputNum(getOutputNum() + 1);

                if ((std::rand() / static_cast<float>(RAND_MAX)) < getBreakdownChance() || getHealth() <= 0) {
                    setRemainingTime(getRepairTime());
                    breakdown();
                }
            } else {
                delete getCurrentProduct(); // 큐 가득 참: 폐기 및 메모리 해제
                setCurrentProduct(nullptr);
                setState(MACHINE_IDLE);
            }
        }
    }
}

std::string Etcher::getInfo() const {
    return "Name: Etcher, State: " + getStateName()
         + ", Queue: " + std::to_string(getQueueSize()) + "/" + std::to_string(getMaxQueueSize())
         + ", Output: " + std::to_string(getOutputNum())
         + ", ProcessTime: " + std::to_string(getProcessTime())
         + ", Health: " + std::to_string(getHealth())
         + "%, Progress: " + std::to_string(getProgress()) + "%";
}

void Etcher::switchCase(Case c) {
    if (c == NORMAL) {
        setProcessTime(4);
    } else if (c == BOTTLENECK) {
        setProcessTime(15);
    }
}