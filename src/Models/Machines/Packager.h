#ifndef PACKAGER_H
#define PACKAGER_H
#include "Machine.h"

class Packager : public Machine {
protected:
    virtual std::shared_ptr<Product> makeWaferPtr(std::shared_ptr<Product>) const override;
public:
    Packager() : Machine(2,4,0.015f) {
        setCaseProcessTimes(2, 2);
    }
    virtual MachineData getInfo() const override;
};
#endif