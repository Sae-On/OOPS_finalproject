#ifndef IONIMPLANTATOR_H
#define IONIMPLANTATOR_H
#include "Models/Machines/Machine.h"

class IonImplantator : public Machine {
public:
    IonImplantator() : Machine(25, 30, 0.08f) {}
    virtual void update(int tick) override;
    virtual std::string getInfo() const override;
    virtual void switchCase(Case c) override;
};
#endif