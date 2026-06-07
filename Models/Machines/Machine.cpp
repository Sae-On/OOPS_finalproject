#include "Models/Machines/Machine.h"

Machine::Machine(int processT, int repairT, float breakdownC)
    : state(MACHINE_IDLE), processTime(processT), output_num(0),
      remaining_time(0), currentProduct(nullptr), nextMachine(nullptr),
      durability(breakdownC, repairT) {}

Machine::~Machine() {
    delete currentProduct;
}

void Machine::breakdown() {
    state = MACHINE_BROKEN;
    durability.breakdown();
}

void Machine::repair() {
    state = MACHINE_IDLE;
    durability.repair();
}

Product* Machine::popQueue() {
    return queue.popQueue();
}

void Machine::addQueue(Product* product) {
    queue.addQueue(product);
}

int Machine::getQueueSize() const {
    return queue.getQueueSize();
}

bool Machine::hasCurrentProduct() const {
    return currentProduct != nullptr;
}

int Machine::popLostProductNum() {
    int count = lost_product_num;
    lost_product_num = 0;
    return count;
}

void Machine::setNextMachine(Machine* next) {
    nextMachine = next;
}

Machine* Machine::getNextMachine() const {
    return nextMachine;
}

int Machine::getMaxQueueSize() const {
    return queue.getMaxQueueSize();
}

int Machine::getDisplayProcessTime() const {
    return getProcessTime();
}

int Machine::getDisplayHealth() const {
    return getHealth();
}

int Machine::getDisplayRemainingTime() const {
    return getRemainingTime();
}

int Machine::getDisplayOutputNum() const {
    return getOutputNum();
}

float Machine::getDisplayBreakdownChance() const {
    return getBreakdownChance();
}

MachineState Machine::getState() const {
    return state;
}

std::string Machine::getStateName() const {
    switch (state) {
        case MACHINE_IDLE:       return "IDLE";
        case MACHINE_PROCESSING: return "PROCESSING";
        case MACHINE_BROKEN:     return "BROKEN";
        default:                 return "UNKNOWN";
    }
}

int Machine::getProcessTime() const {
    return processTime;
}

int Machine::getHealth() const {
    return durability.getHealth();
}

int Machine::getOutputNum() const {
    return output_num;
}

int Machine::getRemainingTime() const {
    return remaining_time;
}

int Machine::getRepairTime() const {
    return durability.getRepairTime();
}

void Machine::setState(MachineState s) {
    state = s;
}

void Machine::setProcessTime(int t) {
    processTime = t;
}

void Machine::setHealth(int h) {
    // durability does not have setHealth directly, we can decrease/increase or skip
}

void Machine::setOutputNum(int n) {
    output_num = n;
}

void Machine::setRemainingTime(int t) {
    remaining_time = t;
}

float Machine::getBreakdownChance() const {
    return durability.getBreakdownChance();
}

Product* Machine::getCurrentProduct() const {
    return currentProduct;
}

void Machine::setCurrentProduct(Product* product) {
    currentProduct = product;
}

int Machine::getProgress() const {
    if (getState() == MACHINE_PROCESSING) {
        int procTime = getProcessTime();
        if (procTime == 0) return 0;
        return 100 - (100 * getRemainingTime()) / getProcessTime();
    } else if (getState() == MACHINE_BROKEN) {
        int repTime = getRepairTime();
        if (repTime == 0) return 0;
        return 100 - (100 * getRemainingTime()) / getRepairTime();
    } else {
        return 0;
    }
}
