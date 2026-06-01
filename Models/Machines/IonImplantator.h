#ifndef IONIMPLANTATOR_H
#define IONIMPLANTATOR_H
#include "Models/Machines/Machine.h"

class IonImplantator : public Machine {
private:
    Machine* nextMachine;
public:
    IonImplantator() : Machine(5,6,0.01f), nextMachine(nullptr) {}
    virtual void update(int tick) override;
    virtual std::string getInfo() const override;
    virtual void switchCase(Case c) override;
    void setNextMachine(Machine* next);
};
#endif