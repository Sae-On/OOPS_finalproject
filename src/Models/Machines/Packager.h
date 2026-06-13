#ifndef PACKAGER_H
#define PACKAGER_H
#include "Machine.h"

class Packager : public Machine {
public:
    Packager() : Machine(2,4,0.015f) {
        setCaseProcessTimes(2, 2);
    }
    virtual void update(int tick) override;
    virtual MachineData getInfo() const override;
};
#endif