#ifndef PHOTOLITHOGRAPHY_H
#define PHOTOLITHOGRAPHY_H
#include "Models/Machines/Machine.h"

class Photolithography : public Machine {
private:
    Machine* nextMachine;
public:
    Photolithography() : Machine(3, 5, 0.01f), nextMachine(nullptr) {}
    virtual void update(int tick) override;
    virtual std::string getInfo() const override;
    virtual void switchCase(Case c) override;
    void setNextMachine(Machine* next);
    Machine* getNextMachine() const { return nextMachine; }
};
#endif