#include "Product.h"

Product::Product(ProductType t) : type(t), state(IDLE) {}

Product::Product(const Product& other) : type(other.type), state(other.state), listener(other.listener) {}

Product::~Product() {}

ProductType Product::getType() const {
    return type;
}

void Product::update(int tick) {
    if ((state==DAMAGED or state==DELETED or state==COMPLETED) and listener) {
        listener->deleteProduct(this, getState());
        listener=nullptr;
    }
}

ProductState Product::getState() const {
    return state;
}

void Product::setState(ProductState s) {
    state = s;
}

void Product::setListener(ProductListener* newListener){
    listener=newListener;
}