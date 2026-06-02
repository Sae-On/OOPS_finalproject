#ifndef IONIMPLANTATOR_H
#define IONIMPLANTATOR_H
#include "Machine.h"

class IonImplantator : public Machine {
private:
    Machine* nextMachine;
public:
    IonImplantator() : Machine(5,6,0.01f), nextMachine(nullptr) {}
    virtual void update(int tick) override;
    virtual MachineData getInfo() const override;
    virtual void switchCase(Case c) override;
};
#endif