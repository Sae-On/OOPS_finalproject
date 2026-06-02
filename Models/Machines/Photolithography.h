#ifndef PHOTOLITHOGRAPHY_H
#define PHOTOLITHOGRAPHY_H
#include "Machine.h"

class Photolithography : public Machine {
public:
    Photolithography() : Machine(3, 5, 0.01f) {}
    virtual void update(int tick) override;
    virtual MachineData getInfo() const override;
    virtual void switchCase(Case c) override;
};
#endif