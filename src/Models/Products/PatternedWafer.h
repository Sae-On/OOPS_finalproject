#ifndef PATTERNEDWAFER_H
#define PATTERNEDWAFER_H
#include "Product.h"

class PatternedWafer : public Product {
public:
    PatternedWafer();
    PatternedWafer(const Product& other);
    std::string getTypeName() const override;
};
#endif