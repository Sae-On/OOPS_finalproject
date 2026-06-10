#ifndef MACHINELISTENER_H
#define MACHINELISTENER_H
#include "../Products/Product.h"
#include <memory>

class MachineListener {
public:
    virtual ~MachineListener() = default;
    virtual void onProductGenerated(std::shared_ptr<Product> newProduct) = 0;
};

#endif