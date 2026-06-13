#ifndef CPU_H
#define CPU_H
#include "Product.h"

class CPU : public Product {
public:
    CPU();
    CPU(const Product& other);
    std::string getTypeName() const override;
};
#endif