#include "Models/Machines/Photolithography.h"
#include "Models/Products/PatternedWafer.h"
#include <cstdlib>

void Photolithography::update(int tick) {
    // 고장 상태: 수리 시간 감소, 완료 시 복구
    if (getState() == MACHINE_BROKEN) {
        setRemainingTime(getRemainingTime() - 1);
        if (getRemainingTime() <= 0) {
            repair();
        }
        return;
    }

    // 유휴 상태: 큐에서 다음 제품 꺼내기
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

    // 처리 중: 남은 시간 감소 및 내구도 소모
    if (getCurrentProduct() != nullptr) {
        setRemainingTime(getRemainingTime() - 1);
        setHealth(getHealth() - 1);

        if (getRemainingTime() <= 0) {
            if (nextMachine && nextMachine->getQueueSize() < nextMachine->getMaxQueueSize()) {
                // 처리 완료: 새 PatternedWafer 생성, 이전 제품 해제
                Product* done = getCurrentProduct();
                bool wasDamaged = (done->getState() == DAMAGED);
                Product* processed = new PatternedWafer(*done);
                delete done; // 메모리 누수 수정: 이전 제품 삭제

                if (wasDamaged || (std::rand() / static_cast<float>(RAND_MAX)) < getBreakdownChance()) {
                    processed->setState(DAMAGED);
                }

                setCurrentProduct(nullptr);
                nextMachine->addQueue(processed);
                setOutputNum(getOutputNum() + 1);

                // 고장 판정
                if ((std::rand() / static_cast<float>(RAND_MAX)) < getBreakdownChance() || getHealth() <= 0) {
                    setRemainingTime(getRepairTime());
                    breakdown();
                }
            } else {
                // 다음 큐가 가득 참: 현재 제품 폐기 (메모리 해제)
                delete getCurrentProduct();
                setCurrentProduct(nullptr);
                setState(MACHINE_IDLE);
            }
        }
    }
}

std::string Photolithography::getInfo() const {
    return "Name: Photolithography, State: " + getStateName()
         + ", Queue: " + std::to_string(getQueueSize()) + "/" + std::to_string(getMaxQueueSize())
         + ", Output: " + std::to_string(getOutputNum())
         + ", ProcessTime: " + std::to_string(getProcessTime())
         + ", Health: " + std::to_string(getHealth())
         + "%, Progress: " + std::to_string(getProgress()) + "%";
}

void Photolithography::switchCase(Case c) {
    // Photolithography는 케이스 전환 없음 (no-op)
}