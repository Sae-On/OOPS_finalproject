#ifndef PRODUCT_H
#define PRODUCT_H
#include <string>
#include "../Root.h"
#include "ProductListener.h"
#include <memory>

enum ProductType { RAW, PATTERNED, ETCHED, DOPED, CPU_TYPE };
enum ProductState : int { IDLE, DELETED, DAMAGED, GENERATED, COMPLETED };

class Product : public Root, public std::enable_shared_from_this<Product> {
private:
    std::weak_ptr<ProductListener> listener;
protected:
    ProductType type;
    ProductState state;
public:
    Product(ProductType t = RAW);
    Product(const Product& other);
    virtual ~Product();
    ProductType getType() const;
    ProductState getState() const;
    void setState(ProductState s);
    virtual std::string getTypeName() const = 0;
    virtual void update(int tick) override;
    void setListener(std::weak_ptr<ProductListener> newListener);
};

#endif