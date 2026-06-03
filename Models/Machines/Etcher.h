#ifndef ETCHER_H
#define ETCHER_H
#include "Machine.h"

class Etcher : public Machine {
public:
    Etcher() : Machine(4, 5, 0.01f) {
        setCaseProcessTimes(4, 15);
    }
    virtual void update(int tick) override;
    virtual MachineData getInfo() const override;
};
#endif