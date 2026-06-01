#ifndef ETCHER_H
#define ETCHER_H
#include "Models/Machines/Machine.h"

class Etcher : public Machine {
public:
    Etcher() : Machine(25, 30, 0.08f) {}
    virtual void update(int tick) override;
    virtual std::string getInfo() const override;
    virtual void switchCase(Case c) override;
};
#endif