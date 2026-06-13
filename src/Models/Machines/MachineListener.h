#ifndef MACHINELISTENER_H
#define MACHINELISTENER_H
#include "../Products/Product.h"
#include <memory>

#include <string>

class MachineListener {
public:
    virtual ~MachineListener() = default;
    virtual void onProductGenerated(std::shared_ptr<Product> newProduct) = 0;
    virtual void onMachineBroken(const std::string& machineName) {}
};

#endif