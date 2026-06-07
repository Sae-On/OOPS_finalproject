#include "FactoryController.h"

FactoryController::FactoryController() {
    // 공정 체인 연결: Photolithography -> Etcher -> IonImplantator -> Packager
    machines_.push_back(new Photolithography());
    machines_.push_back(new Etcher());
    machines_.push_back(new IonImplantator());
    machines_.push_back(new Packager());

    power_ = {true, true, true, true};

    for(size_t i = 0; i < machines_.size() - 1; ++i) {
        machines_[i]->setNextMachine(machines_[i+1]);
    }
}

FactoryController::~FactoryController() {
    for (Machine* m : machines_) {
        delete m;
    }
    machines_.clear();
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
    tick_ = 0;
    totalBreakdownCount_ = 0;
    lostProductCount_ = 0;
    eventLog_.clear();

    for (Machine* m : machines_) {
        delete m;
    }
    machines_.clear();

    machines_.push_back(new Photolithography());
    machines_.push_back(new Etcher());
    machines_.push_back(new IonImplantator());
    machines_.push_back(new Packager());

    for(size_t i = 0; i < machines_.size() - 1; ++i) {
        machines_[i]->setNextMachine(machines_[i+1]);
    }

    power_ = {true, true, true, true};
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

    tick_++;

    // 원자재 투입: 첫 번째 장비 큐에 RawWafer 넣기
    if (rawWaferCount_ > 0 && !machines_.empty() &&
        machines_[0]->getQueueSize() < machines_[0]->getMaxQueueSize()) {
        machines_[0]->addQueue(new RawWafer());
        rawWaferCount_--;
    }

    // 각 장비를 순서대로 한 틱 업데이트 (전원 on인 경우에만)
    for (size_t i = 0; i < machines_.size(); ++i) {
        if (i < power_.size() && power_[i]) {
            MachineState oldState = machines_[i]->getStateName() == "BROKEN" ? MACHINE_BROKEN : 
                                    (machines_[i]->getStateName() == "PROCESSING" ? MACHINE_PROCESSING : MACHINE_IDLE);

            machines_[i]->update(0);

            MachineState newState = machines_[i]->getStateName() == "BROKEN" ? MACHINE_BROKEN : 
                                    (machines_[i]->getStateName() == "PROCESSING" ? MACHINE_PROCESSING : MACHINE_IDLE);

            if (oldState != MACHINE_BROKEN && newState == MACHINE_BROKEN) {
                eventLog_.push_back("[Tick " + std::to_string(tick_) + "] " + getMachineName(i) + " broke down!");
                totalBreakdownCount_++;
            } else if (oldState == MACHINE_BROKEN && newState != MACHINE_BROKEN) {
                eventLog_.push_back("[Tick " + std::to_string(tick_) + "] " + getMachineName(i) + " was repaired.");
            }

            int lost = machines_[i]->popLostProductNum();
            if (lost > 0) {
                lostProductCount_ += lost;
                eventLog_.push_back("[Tick " + std::to_string(tick_) + "] " + getMachineName(i) + " overflow! Lost " + std::to_string(lost) + " products.");
            }
        }
    }

    collectFinishedProducts();
}

void FactoryController::updateCase(Case c) {
    for (Machine* m : machines_) {
        m->switchCase(c);
    }
    std::string caseName = (c == NORMAL) ? "NORMAL" : "BOTTLENECK";
    eventLog_.push_back("[Scenario] Changed to " + caseName);
}

void FactoryController::forceBreakMachine(int index) {
    if (index >= 0 && index < machines_.size()) {
        machines_[index]->breakdown();
        // The update() loop will catch this state change and log it.
    }
}

// ── 상태 조회 ───────────────────────────────────────────────

bool FactoryController::isRunning() const {
    return running_;
}

Machine* FactoryController::getMachine(int index) {
    if (index >= 0 && index < machines_.size()) {
        return machines_[index];
    }
    return nullptr;
}

const Machine* FactoryController::getMachine(int index) const {
    if (index >= 0 && index < machines_.size()) {
        return machines_[index];
    }
    return nullptr;
}

int FactoryController::getTick() const { return tick_; }
int FactoryController::getTotalBreakdownCount() const { return totalBreakdownCount_; }
int FactoryController::getLostProductCount() const { return lostProductCount_; }
std::vector<std::string> FactoryController::popEventLogs() {
    std::vector<std::string> logs = eventLog_;
    eventLog_.clear();
    return logs;
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
    if (machines_.size() > 0 && machines_[0]) {
        count += machines_[0]->getQueueSize();
        if (machines_[0]->hasCurrentProduct()) {
            count += 1;
        }
    }
    return count;
}

int FactoryController::getPatternedWaferCount() const {
    int count = 0;
    if (machines_.size() > 1 && machines_[1]) {
        count += machines_[1]->getQueueSize();
        if (machines_[1]->hasCurrentProduct()) {
            count += 1;
        }
    }
    return count;
}

int FactoryController::getEtchedWaferCount() const {
    int count = 0;
    if (machines_.size() > 2 && machines_[2]) {
        count += machines_[2]->getQueueSize();
        if (machines_[2]->hasCurrentProduct()) {
            count += 1;
        }
    }
    return count;
}

int FactoryController::getDopedWaferCount() const {
    int count = 0;
    if (machines_.size() > 3 && machines_[3]) {
        count += machines_[3]->getQueueSize();
        if (machines_[3]->hasCurrentProduct()) {
            count += 1;
        }
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
    if (machines_.size() < 4) return;
    Packager* p = dynamic_cast<Packager*>(machines_[3]);
    if (!p) return;
    
    std::vector<Product*> cpus = p->popFinishedProducts();
    for (Product* cpu : cpus) {
        if (cpu->getState() == DAMAGED) {
            defectiveCount_++;
        } else {
            finishedCPUCount_++;
        }
        delete cpu;
    }
}
