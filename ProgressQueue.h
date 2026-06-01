#ifndef PROGRESSQUEUE_H
#define PROGRESSQUEUE_H
#include <vector>
#include "Models/Products/Product.h"
class ProgressQueue{
private:
    std::vector<Product*> queue;
    int maxQueueSize=6;
public:
    ~ProgressQueue();
    Product* popQueue();
    void addQueue(Product* product);
    int getQueueSize() const;
    int getMaxQueueSize() const;
};

#endif