#ifndef PHOTOLITHOGRAPHY_H
#define PHOTOLITHOGRAPHY_H
#include "Machine.h"

class Photolithography : public Machine {
private:
    Machine* nextMachine;
public:
    Photolithography() : Machine(3, 5, 0.1f) {}

    virtual void update(int tick) override;
    virtual std::string getInfo() const override;
    virtual void switchCase(int caseNum) override;
};
#endif