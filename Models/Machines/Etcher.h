#ifndef ETCHER_H
#define ETCHER_H
#include "Models/Machines/Machine.h"

class Etcher : public Machine {
private:
    Machine* nextMachine;
public:
    Etcher() : Machine(4, 5, 0.01f) {}

    virtual void update(int tick) override;
    virtual std::string getInfo() const override;
    virtual void switchCase(Case c) override;
    void setNextMachine(Machine* next);
};
#endif