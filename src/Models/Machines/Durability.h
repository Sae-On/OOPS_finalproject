#ifndef DURABILITY_H
#define DURABILITY_H

class Durability {
private:
    int health;
    int const MAX_HEALTH=200;
    int const REPAIR_ADD_HEALTH=100;
    float breakdown_chance;
    int repair_time;
public:
    Durability(float bc, int rt);
    void decreaseHealth(int amount);
    bool checkBreakdown();
    void repair();
    void breakdown();
    int getHealth() const;
    int getMaxHealth() const;
    int getRepairTime() const;
    void reset();
    float getBreakdownChance() const { return breakdown_chance; }
    void setBreakdownChance(float bc) { breakdown_chance = bc; }
};

#endif