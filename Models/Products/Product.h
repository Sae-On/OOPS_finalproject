#ifndef PRODUCT_H
#define PRODUCT_H
#include <string>
#include "../Root.h"
#include "ProductListener.h"

enum ProductType { RAW, PATTERNED, ETCHED, DOPED, CPU_TYPE };
enum ProductState : int { IDLE, DELETED, DAMAGED, GENERATED, COMPLETED };

class Product : public Root {
private:
    ProductListener* listener = nullptr;
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
    void setListener(ProductListener* newListener);
};

#endif