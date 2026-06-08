#include "Machine.h"

Machine::Machine(int processT, int repairT, float breakdownC)
    : state(MACHINE_IDLE), processTime(processT), output_num(0), remaining_time(0), currentProduct(nullptr), durability(breakdownC, repairT), normalProcessTime(processT), bottleneckProcessTime(processT) {}

Machine::~Machine() {
    delete currentProduct;
}

void Machine::switchCase(Case c) {
    setProcessTime(getProcessTimeForCase(c));
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

void Machine::setListener(MachineListener* newListener){
    listener=newListener;
}

Product* Machine::generateProduct(Product* new_product){
    if (new_product && listener){
        listener->onProductGenerated(new_product);
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
    durability.reset();
    queue.reset();
}
int Machine::getProcessTimeForCase(Case c) const {
    if (c == BOTTLENECK) {
        return bottleneckProcessTime;
    }
    return normalProcessTime;
}