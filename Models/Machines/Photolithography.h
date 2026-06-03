#ifndef PHOTOLITHOGRAPHY_H
#define PHOTOLITHOGRAPHY_H
#include "Machine.h"

class Photolithography : public Machine {
public:
    Photolithography() : Machine(3, 5, 0.01f) {
        setCaseProcessTimes(3, 3);
    }
    virtual void update(int tick) override;
    virtual MachineData getInfo() const override;
};
#endif