#ifndef RAWWAFER_H
#define RAWWAFER_H
#include "Product.h"

class RawWafer : public Product {
public:
    RawWafer();
    std::string getTypeName() const override;
};
#endif