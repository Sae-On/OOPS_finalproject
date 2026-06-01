#ifndef PHOTOLITHOGRAPHY_H
#define PHOTOLITHOGRAPHY_H
#include "Models/Machines/Machine.h"

class Photolithography : public Machine {
public:
    Photolithography() : Machine(15, 20, 0.10f) {}
    virtual void update(int tick) override;
    virtual std::string getInfo() const override;
    virtual void switchCase(Case c) override;
};
#endif