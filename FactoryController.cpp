#include "FactoryController.h"

FactoryController::FactoryController() {
    // 공정 체인 연결: Photolithography -> Etcher -> IonImplantator -> Packager
    photolithography_.setNextMachine(&etcher_);
    etcher_.setNextMachine(&ionImplantator_);
    ionImplantator_.setNextMachine(&packager_);
    // Packager는 마지막 단계이므로 nextMachine 없음
}

// ── 제어 인터페이스 ─────────────────────────────────────────

void FactoryController::start() {
    running_ = true;
}

void FactoryController::pause() {
    running_ = false;
}

void FactoryController::reset() {
    running_ = false;
    rawWaferCount_    = 0;
    finishedCPUCount_ = 0;
    defectiveCount_   = 0;

    // 장비 재초기화 (새 객체로 교체)
    photolithography_ = Photolithography();
    etcher_           = Etcher();
    ionImplantator_   = IonImplantator();
    packager_         = Packager();

    // 체인 재연결
    photolithography_.setNextMachine(&etcher_);
    etcher_.setNextMachine(&ionImplantator_);
    ionImplantator_.setNextMachine(&packager_);

    for (int i = 0; i < 4; ++i) power_[i] = true;
}

void FactoryController::addRawWafer(int count) {
    rawWaferCount_ += count;
}

void FactoryController::repairMachine(int index) {
    Machine* m = getMachine(index);
    if (m) m->repair();
}

void FactoryController::setPower(int index, bool on) {
    if (index < 0 || index >= 4) return;
    power_[index] = on;
}

// ── 시뮬레이션 업데이트 ─────────────────────────────────────

void FactoryController::update() {
    if (!running_) return;

    // 원자재 투입: 첫 번째 장비 큐에 RawWafer 넣기
    if (rawWaferCount_ > 0 &&
        photolithography_.getQueueSize() < photolithography_.getMaxQueueSize()) {
        photolithography_.addQueue(new RawWafer());
        rawWaferCount_--;
    }

    // 각 장비를 순서대로 한 틱 업데이트 (전원 on인 경우에만)
    if (power_[0]) photolithography_.update(0);
    if (power_[1]) etcher_.update(0);
    if (power_[2]) ionImplantator_.update(0);
    if (power_[3]) packager_.update(0);

    collectFinishedProducts();
}

// ── 상태 조회 ───────────────────────────────────────────────

bool FactoryController::isRunning() const {
    return running_;
}

Machine* FactoryController::getMachine(int index) {
    switch (index) {
        case 0: return &photolithography_;
        case 1: return &etcher_;
        case 2: return &ionImplantator_;
        case 3: return &packager_;
        default: return nullptr;
    }
}

const Machine* FactoryController::getMachine(int index) const {
    switch (index) {
        case 0: return &photolithography_;
        case 1: return &etcher_;
        case 2: return &ionImplantator_;
        case 3: return &packager_;
        default: return nullptr;
    }
}

static const char* kMachineNames[4] = {
    "Photolithography", "Etcher", "IonImplantator", "Packager"
};

std::string FactoryController::getMachineName(int index) const {
    if (index < 0 || index >= 4) return "";
    return kMachineNames[index];
}

std::string FactoryController::getMachineState(int index) const {
    const Machine* m = getMachine(index);
    if (!m) return "";
    return m->getStateName();
}

int FactoryController::getMachineHealth(int index) const {
    const Machine* m = getMachine(index);
    return m ? m->getDisplayHealth() : 0;
}

int FactoryController::getMachineProgress(int index) const {
    const Machine* m = getMachine(index);
    return m ? m->getProgress() : 0;
}

float FactoryController::getMachineBreakdownChance(int index) const {
    const Machine* m = getMachine(index);
    return m ? m->getDisplayBreakdownChance() : 0.0f;
}

int FactoryController::getMachineProcessTime(int index) const {
    const Machine* m = getMachine(index);
    return m ? m->getDisplayProcessTime() : 0;
}

int FactoryController::getMachineRemainingTime(int index) const {
    const Machine* m = getMachine(index);
    return m ? m->getDisplayRemainingTime() : 0;
}

int FactoryController::getMachineQueueSize(int index) const {
    const Machine* m = getMachine(index);
    return m ? m->getQueueSize() : 0;
}

int FactoryController::getMachineOutputNum(int index) const {
    const Machine* m = getMachine(index);
    return m ? m->getDisplayOutputNum() : 0;
}

bool FactoryController::getMachinePower(int index) const {
    if (index < 0 || index >= 4) return false;
    return power_[index];
}

int FactoryController::getRawWaferCount() const {
    int count = rawWaferCount_;
    count += photolithography_.getQueueSize();
    if (photolithography_.hasCurrentProduct()) {
        count += 1;
    }
    return count;
}

int FactoryController::getPatternedWaferCount() const {
    int count = etcher_.getQueueSize();
    if (etcher_.hasCurrentProduct()) {
        count += 1;
    }
    return count;
}

int FactoryController::getEtchedWaferCount() const {
    int count = ionImplantator_.getQueueSize();
    if (ionImplantator_.hasCurrentProduct()) {
        count += 1;
    }
    return count;
}

int FactoryController::getDopedWaferCount() const {
    int count = packager_.getQueueSize();
    if (packager_.hasCurrentProduct()) {
        count += 1;
    }
    return count;
}

int FactoryController::getFinishedCPUCount() const {
    return finishedCPUCount_;
}

int FactoryController::getDefectiveCount() const {
    return defectiveCount_;
}

void FactoryController::collectFinishedProducts() {
    // Packager에서 완성된 CPU를 수거하여 카운트
    std::vector<Product*> cpus = packager_.popFinishedProducts();
    for (Product* p : cpus) {
        if (p->getState() == DAMAGED) {
            defectiveCount_++;
        } else {
            finishedCPUCount_++;
        }
        delete p; // 수거 후 메모리 해제
    }
}
