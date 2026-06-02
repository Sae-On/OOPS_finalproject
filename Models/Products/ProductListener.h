#ifndef PRODUCTLISTENER_H
#define PRODUCTLISTENER_H

class Product;
enum ProductState : int;

class ProductListener {
public:
    virtual ~ProductListener() = default;
    virtual void deleteProduct(Product* product, ProductState state) = 0;
};

#endif