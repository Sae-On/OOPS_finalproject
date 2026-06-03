#include "Durability.h"
#include <random>

Durability::Durability(float bc, int rt):breakdown_chance(bc), repair_time(rt){}

void Durability::decreaseHealth(int amount){
    if (health > amount) {
        health-=amount;
    } else {
        health=0;
    }
}

bool Durability::checkBreakdown(){
    return (std::rand() / static_cast<float>(RAND_MAX)) < breakdown_chance || health <= 0;
}

int Durability::getHealth() const{
    return health;
}

void Durability::repair(){
    health = health > 70 ? 100 : health + 30;
}

int Durability::getRepairTime() const{
    return repair_time;
}

void Durability::reset() {
    health = 100;
}

void Durability::breakdown() {
    decreaseHealth(20);
}