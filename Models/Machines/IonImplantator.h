#ifndef IONIMPLANTATOR_H
#define IONIMPLANTATOR_H
#include "Machine.h"

class IonImplantator : public Machine {
public:
    IonImplantator() : Machine(5,6,0.01f) {
        setCaseProcessTimes(5, 5);
    }
    virtual void update(int tick) override;
    virtual MachineData getInfo() const override;
};
#endif