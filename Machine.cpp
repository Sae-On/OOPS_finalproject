#include "Machine.h"

Machine::Machine(int processT, int repairT, float breakdownC)
    : state(STATE_IDLE), processTime(processT), health(100), output_num(0), remaining_time(0), repair_time(repairT), breakdown_chance(breakdownC), currentProduct(nullptr) {}

Machine::~Machine() {
    for (Product* product : queue) {
        delete product;
    }
    delete currentProduct;
    queue.clear();
}

void Machine::breakdown() {
    state = STATE_BROKEN;
}

void Machine::repair() {
    state = STATE_IDLE;
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

// Getter implementations
int Machine::getState() const {
    return state;
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

// Setter implementations
void Machine::setState(int s) {
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

void Machine::setRepairTime(int t) {
    repair_time = t;
}

float Machine::getBreakdownChance() const {
    return breakdown_chance;
}

void Machine::setBreakdownChance(float c) {
    breakdown_chance = c;
}

Product* Machine::getCurrentProduct() const {
    return currentProduct;
}

void Machine::setCurrentProduct(Product* product) {
    currentProduct = product;
}