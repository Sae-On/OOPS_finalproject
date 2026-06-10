#include "Durability.h"
#include <random>

Durability::Durability(float bc, int rt):breakdown_chance(bc), repair_time(rt){
    health=MAX_HEALTH;
}

void Durability::decreaseHealth(int amount){
    if (health > amount) {
        health-=amount;
    } else {
        health=0;
    }
}

bool Durability::checkBreakdown(){
    return (std::rand() / static_cast<float>(RAND_MAX)) < breakdown_chance or health <= 0;
}

int Durability::getHealth() const{
    return health;
}

int Durability::getMaxHealth() const{
    return MAX_HEALTH;
}

void Durability::repair(){
    health = health > MAX_HEALTH-REPAIR_ADD_HEALTH ? MAX_HEALTH : health+REPAIR_ADD_HEALTH;
}

int Durability::getRepairTime() const{
    return repair_time;
}

void Durability::reset() {
    health = 200;
}

void Durability::breakdown() {
    decreaseHealth(20);
}