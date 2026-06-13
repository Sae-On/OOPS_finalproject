#ifndef PHOTOLITHOGRAPHY_H
#define PHOTOLITHOGRAPHY_H
#include "Machine.h"

class Photolithography : public Machine {
protected:
    virtual std::shared_ptr<Product> makeWaferPtr(std::shared_ptr<Product>) const override;
public:
    Photolithography() : Machine(3, 5, 0.02f) {
        setCaseProcessTimes(3, 3);
    }
    virtual MachineData getInfo() const override;
};
#endif