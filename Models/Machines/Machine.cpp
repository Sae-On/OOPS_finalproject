#include "Models/Machines/Machine.h"

Machine::Machine(int processT, int repairT, float breakdownC)
    : state(IDLE), processTime(processT), health(100), output_num(0), remaining_time(0), repair_time(repairT), breakdown_chance(breakdownC), currentProduct(nullptr) {}

Machine::~Machine() {
    for (Product* product : queue) {
        delete product;
    }
    delete currentProduct;
    queue.clear();
}

void Machine::breakdown() {
    state = BROKEN;
}

void Machine::repair() {
    state = IDLE;
    health = health > 70 ? 100 : health + 40;
}

Product* Machine::popQueue() {
    if (queue.empty()) {
        return nullptr;
    }
    Product* product = queue.front();
    queue.erase(queue.begin());
    return product;
}

void Machine::addQueue(Product* product){
    queue.push_back(product);
}

int Machine::getQueueSize() const {
    return queue.size();
}

int Machine::getMaxQueueSize() const {
    return maxQueueSize;
}

MachineState Machine::getState() const {
    return state;
}

std::string Machine::getStateName() const {
    switch (state) {
        case IDLE: return "IDLE";
        case PROCESSING: return "PROCESSING";
        case BROKEN: return "BROKEN";
        default: return "UNKNOWN";
    }
}

int Machine::getProcessTime() const {
    return processTime;
}

int Machine::getHealth() const {
    return health;
}

int Machine::getOutputNum() const {
    return output_num;
}

int Machine::getRemainingTime() const {
    return remaining_time;
}

int Machine::getRepairTime() const {
    return repair_time;
}

void Machine::setState(MachineState s) {
    state = s;
}



void Machine::setProcessTime(int t) {
    processTime = t;
}

void Machine::setHealth(int h) {
    if (h < 0) h = 0;
    if (h > 100) h = 100;
    health = h;
}

void Machine::setOutputNum(int n) {
    output_num = n;
}

void Machine::setRemainingTime(int t) {
    remaining_time = t;
}

float Machine::getBreakdownChance() const {
    return breakdown_chance;
}

Product* Machine::getCurrentProduct() const {
    return currentProduct;
}

void Machine::setCurrentProduct(Product* product) {
    currentProduct = product;
}

int Machine::getProgress() const {
    if (getState() == PROCESSING) {
        int procTime = getProcessTime();
        if (procTime == 0) return 0;
        return 100 - (100 * getRemainingTime()) / getProcessTime();
    } else if (getState() == BROKEN) {
        int repTime = getRepairTime();
        if (repTime == 0) return 0;
        return 100 - (100 * getRemainingTime()) / getRepairTime();
    } else {
        return 0;
    }
}