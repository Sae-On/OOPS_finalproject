#include "ProgressQueue.h"
Product* ProgressQueue::popQueue() {
    if (queue.empty()) {
        return nullptr;
    }
    Product* product = queue.front();
    queue.erase(queue.begin());
    return product;
}

ProgressQueue::~ProgressQueue() {
    queue.clear();
}

void ProgressQueue::addQueue(Product* product){
    queue.push_back(product);
}

int ProgressQueue::getQueueSize() const {
    return queue.size();
}

int ProgressQueue::getMaxQueueSize() const {
    return maxQueueSize;
}