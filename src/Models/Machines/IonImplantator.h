#ifndef IONIMPLANTATOR_H
#define IONIMPLANTATOR_H
#include "Machine.h"

class IonImplantator : public Machine {
protected:
    virtual std::shared_ptr<Product> makeWaferPtr(std::shared_ptr<Product>) const override;
public:
    IonImplantator() : Machine(5,6,0.01f) {
        setCaseProcessTimes(5, 5);
    }
    virtual MachineData getInfo() const override;
};
#endif