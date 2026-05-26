#ifndef DOPEDWAFER_H
#define DOPEDWAFER_H
#include "Models/Products/Product.h"

class DopedWafer : public Product {
public:
    DopedWafer();
    DopedWafer(const Product& other);
    std::string getTypeName() const override;
};
#endif