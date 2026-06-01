#ifndef DURABILITY_H
#define DURABILITY_H

class Durability {
private:
    int health = 100;
    float breakdown_chance;
    int repair_time;
public:
    Durability(float bc, int rt);
    void decreaseHealth(int amount);
    bool checkBreakdown();
    void repair();
    void breakdown();
    int getHealth() const;
    int getRepairTime() const;
};

#endif