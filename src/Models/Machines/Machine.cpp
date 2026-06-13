#include "Machine.h"

Machine::Machine(int processT, int repairT, float breakdownC)
    : state(MACHINE_IDLE), processTime(processT), output_num(0), remaining_time(0), currentProduct(nullptr), durability(breakdownC, repairT), normalProcessTime(processT), bottleneckProcessTime(processT), powered(true), baseBreakdownChance(breakdownC) {}


void Machine::switchCase(Case c) {
    setProcessTime(getProcessTimeForCase(c));
    // Random Breakdowns scenario raises the breakdown probability for every machine.
    durability.setBreakdownChance(c == BREAKDOWN ? 0.06f : baseBreakdownChance);
}

void Machine::breakdown() {
    setState(MACHINE_BROKEN);
    remaining_time=durability.getRepairTime();
}

std::string Machine::getStateName() const {
    switch (state) {
        case MACHINE_IDLE: return "IDLE";
        case MACHINE_PROCESSING: return "PROCESSING";
        case MACHINE_BROKEN: return "BROKEN";
        default: return "UNKNOWN";
    }
}

int Machine::getProgress() const {
    if (getState() == MACHINE_PROCESSING) {
        int procTime = getProcessTime();
        if (procTime == 0) return 0;
        return 100 - (100 * remaining_time) / getProcessTime();
    } else if (getState() == MACHINE_BROKEN) {
        int repTime = durability.getRepairTime();
        if (repTime == 0) return 0;
        return 100 - (100 * remaining_time) / durability.getRepairTime();
    } else {
        return 0;
    }
}

ProgressQueue &Machine::getQueue() {
    return queue;
}

void Machine::setListener(std::weak_ptr<MachineListener> newListener){
    listener=newListener;
}

std::shared_ptr<Product> Machine::generateProduct(std::shared_ptr<Product> new_product){
    if (new_product){
        if (auto listener_shared=listener.lock()){
            listener_shared->onProductGenerated(new_product);
        }
    }
    return new_product;
}

void Machine::handleBrokenState(){
    decreaseRemainingTime(1);
    if (!isRemainTime()) {
        repair();
        fetchNextProduct();
    }
}
void Machine::fetchNextProduct() {
    setCurrentProduct(queue.popQueue());
    if (getCurrentProduct()) {
        setState(MACHINE_PROCESSING);
        remaining_time=getProcessTime();
    } else {
        setState(MACHINE_IDLE);
    }
}

void Machine::repair() {
    durability.repair();
    setState(MACHINE_IDLE);
}

void Machine::decreaseRemainingTime(int amount){
    remaining_time-=amount;
}

bool Machine::isRemainTime() const{
    return remaining_time>0;
}

void Machine::reset() {
    setState(MACHINE_IDLE);
    setCurrentProduct(nullptr);
    output_num = 0;
    remaining_time = 0;
    powered = true;
    durability.reset();
    durability.setBreakdownChance(baseBreakdownChance);
    queue.reset();
}
int Machine::getProcessTimeForCase(Case c) const {
    if (c == BOTTLENECK) {
        return bottleneckProcessTime;
    }
    return normalProcessTime;
}

void Machine::handleProcessing(){
    auto next=getNextMachine();
    decreaseRemainingTime(1);
    durability.decreaseHealth(1);
    if (!isRemainTime()) {
        if (!next) {
            std::shared_ptr<Product> done = getCurrentProduct();
            setCurrentProduct(nullptr);
            done->setState(DELETED);
            auto newWafer=makeWaferPtr(done);
            generateProduct(newWafer);
            setOutputNum(getOutputNum() + 1);
            if (durability.checkBreakdown()) {
                breakdown();
            }
        } else if (next && next->getQueue().getQueueSize() < next->getQueue().getMaxQueueSize()) {
            std::shared_ptr<Product> done = getCurrentProduct();
            auto newWafer=makeWaferPtr(done);
            std::shared_ptr<Product> processedProduct = generateProduct(newWafer);
            setCurrentProduct(nullptr);
            done->setState(DELETED);
            next->getQueue().addQueue(processedProduct);
            
            setOutputNum(getOutputNum() + 1);
            if (durability.checkBreakdown()) {
                breakdown();
            }
        } else {
            getCurrentProduct()->setState(DAMAGED);
            setState(MACHINE_IDLE);
            setCurrentProduct(nullptr);
        }
    }
}

void Machine::update(int tick) {
    if (!power) return;
    if (getState() == MACHINE_BROKEN) {
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
        handleProcessing();
    }  
}

void Machine::setPower(bool power){
    this->power=power;
}