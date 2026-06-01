#ifndef GENERATOR_H
#define GENERATOR_H
#include "Models/Root.h"
#include "Models/Products/RawWafer.h"
class Generator : public Root {
private:
    int generate_time;
    int remain_time;
    void (*onElementGenerated)(Root*);
    int lost_products_num=0;
    int completed_products_num=0;
public:
    Generator();
    void update(int tick) override;
    void setGenerationCallback(void (*cb)(Root*));
};
#endif