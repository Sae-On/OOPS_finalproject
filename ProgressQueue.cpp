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
    for (Product* p : queue) {
        delete p;
    }
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

void ProgressQueue::reset() {
    queue.clear();
}