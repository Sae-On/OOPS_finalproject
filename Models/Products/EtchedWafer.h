#ifndef ETCHEDWAFER_H
#define ETCHEDWAFER_H
#include "Models/Products/Product.h"

class EtchedWafer : public Product {
public:
    EtchedWafer();
    EtchedWafer(const Product& other);
    std::string getTypeName() const override;
};
#endif