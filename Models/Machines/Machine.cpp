#include "Models/Machines/Machine.h"

Machine::Machine(int processT, int repairT, float breakdownC)
    : state(IDLE), processTime(processT), output_num(0), remaining_time(0), currentProduct(nullptr), durability(breakdownC, repairT) {}

Machine::~Machine() {
    delete currentProduct;
}

void Machine::breakdown() {
    durability.decreaseHealth(20);
    setState(BROKEN);
    remaining_time=durability.getRepairTime();
}

std::string Machine::getStateName() const {
    switch (state) {
        case IDLE: return "IDLE";
        case PROCESSING: return "PROCESSING";
        case BROKEN: return "BROKEN";
        default: return "UNKNOWN";
    }
}

int Machine::getProgress() const {
    if (getState() == PROCESSING) {
        int procTime = getProcessTime();
        if (procTime == 0) return 0;
        return 100 - (100 * remaining_time) / getProcessTime();
    } else if (getState() == BROKEN) {
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
    if (new_product and listener){
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
        setState(PROCESSING);
        remaining_time=getProcessTime();
    } else {
        setState(IDLE);
    }
}

void Machine::repair() {
    durability.repair();
    setState(IDLE);
}

void Machine::decreaseRemainingTime(int amount){
    remaining_time-=amount;
}

bool Machine::isRemainTime() const{
    return remaining_time>0;
}