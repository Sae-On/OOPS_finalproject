#ifndef PRODUCTLISTENER_H
#define PRODUCTLISTENER_H
#include "Models/Products/Product.h"

class ProductListener {
public:
    virtual ~ProductListener() = default;
    virtual void deleteProduct(Product* product, ProductState state) = 0;
};

#endif