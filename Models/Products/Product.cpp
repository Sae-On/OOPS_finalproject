#include "Models/Products/Product.h"

Product::Product(ProductType t) : type(t), state(GENERATED) {}

Product::Product(const Product& other) : type(other.type), state(other.state) {}

Product::~Product() {}

ProductType Product::getType() const {
    return type;
}

void Product::update(int tick) {
    //TODO: Controller에 Product객체 상태 변화 알림
}

ProductState Product::getState() const {
    return state;
}

void Product::setState(ProductState s) {
    state = s;
}