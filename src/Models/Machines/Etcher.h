#ifndef ETCHER_H
#define ETCHER_H
#include "Machine.h"

class Etcher : public Machine {
protected:
    virtual std::shared_ptr<Product> makeWaferPtr(std::shared_ptr<Product>) const override;
public:
    Etcher() : Machine(4, 5, 0.01f) {
        setCaseProcessTimes(4, 15);
    }
    virtual MachineData getInfo() const override;
};
#endif