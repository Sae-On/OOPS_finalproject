#ifndef PRODUCTLISTENER_H
#define PRODUCTLISTENER_H
#include <memory>

class Product;
enum ProductState : int;

class ProductListener {
public:
    virtual ~ProductListener() = default;
    virtual void deleteProduct(std::shared_ptr<Product> product, ProductState state) = 0;
};

#endif