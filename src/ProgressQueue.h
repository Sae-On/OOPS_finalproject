#ifndef PROGRESSQUEUE_H
#define PROGRESSQUEUE_H
#include <vector>
#include "Models/Products/Product.h"
#include <memory>
class ProgressQueue{
private:
    std::vector<std::shared_ptr<Product>> queue;
    int maxQueueSize=6;
public:
    ~ProgressQueue();
    std::shared_ptr<Product> popQueue();
    void addQueue(std::shared_ptr<Product> product);
    int getQueueSize() const;
    int getMaxQueueSize() const;
    void reset();
};

#endif