#include "ProgressQueue.h"
std::shared_ptr<Product> ProgressQueue::popQueue() {
    if (queue.empty()) {
        return nullptr;
    }
    std::shared_ptr<Product> product = queue.front();
    queue.erase(queue.begin());
    return product;
}

ProgressQueue::~ProgressQueue() {
    queue.clear();
}

void ProgressQueue::addQueue(std::shared_ptr<Product> product){
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