#include "Models/Machines/Packager.h"
#include "Models/Products/CPU.h"
#include <cstdlib>

Packager::~Packager() {
    for (Product* p : finishedProducts) {
        delete p;
    }
    finishedProducts.clear();
}

void Packager::update(int tick) {
    if (getState() == MACHINE_BROKEN) {
        setRemainingTime(getRemainingTime() - 1);
        if (getRemainingTime() <= 0) repair();
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
            // CPU 완성품 생성 (기존 누락 로직 추가)
            Product* done = getCurrentProduct();
            bool wasDamaged = (done->getState() == DAMAGED);
            Product* cpu = new CPU(*done);
            delete done; // 메모리 누수 수정: 이전 단계 제품 삭제

            if (wasDamaged || (std::rand() / static_cast<float>(RAND_MAX)) < getBreakdownChance()) {
                cpu->setState(DAMAGED);
            }

            setCurrentProduct(nullptr);

            finishedProducts.push_back(cpu);
            setOutputNum(getOutputNum() + 1);

            if ((std::rand() / static_cast<float>(RAND_MAX)) < getBreakdownChance() || getHealth() <= 0) {
                setRemainingTime(getRepairTime());
                breakdown();
            }
        }
    }
}

std::string Packager::getInfo() const {
    return "Name: Packager, State: " + getStateName()
         + ", Queue: " + std::to_string(getQueueSize()) + "/" + std::to_string(getMaxQueueSize())
         + ", Output: " + std::to_string(getOutputNum())
         + ", ProcessTime: " + std::to_string(getProcessTime())
         + ", Health: " + std::to_string(getHealth())
         + "%, Progress: " + std::to_string(getProgress()) + "%"
         + ", Finished CPUs: " + std::to_string(getFinishedCount());
}

void Packager::switchCase(Case c) {
    // no-op
}

int Packager::getFinishedCount() const {
    return static_cast<int>(finishedProducts.size());
}

std::vector<Product*> Packager::popFinishedProducts() {
    std::vector<Product*> result = std::move(finishedProducts);
    finishedProducts.clear();
    return result;
}
