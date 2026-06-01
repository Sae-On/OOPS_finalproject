#ifndef MACHINELISTENER_H
#define MACHINELISTENER_H
#include "Models/Products/Product.h"

class MachineListener {
public:
    virtual ~MachineListener() = default;
    virtual void onProductGenerated(Product* newProduct) = 0;
};

#endif