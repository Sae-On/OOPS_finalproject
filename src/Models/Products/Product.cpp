#include "Product.h"

Product::Product(ProductType t) : type(t), state(IDLE) {}

Product::Product(const Product& other) : type(other.type), state(other.state), listener(other.listener) {}

Product::~Product() {}

ProductType Product::getType() const {
    return type;
}

void Product::update(int tick) {
    if ((state==DAMAGED or state==DELETED or state==COMPLETED)) {
        if (auto shared_listener=listener.lock()){
            shared_listener->deleteProduct(shared_from_this(), getState());
        }
    }
    listener.reset();
}

ProductState Product::getState() const {
    return state;
}

void Product::setState(ProductState s) {
    state = s;
}

void Product::setListener(std::weak_ptr<ProductListener> newListener){
    listener=newListener;
}