#ifndef PACKAGER_H
#define PACKAGER_H
#include "Machine.h"

class Packager : public Machine {
public:
    Packager() : Machine(2,4,0.01f) {}
    virtual void update(int tick) override;
    virtual MachineData getInfo() const override;
    virtual void switchCase(Case c) override;
};
#endif