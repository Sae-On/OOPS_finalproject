#ifndef MACHINE_H
#define MACHINE_H
#include <string>
#include <vector>
#include "../Products/Product.h"
#include "../Root.h"
#include "../../ProgressQueue.h"
#include "Durability.h"
#include "MachineListener.h"
#include <memory>

enum MachineState { MACHINE_IDLE, MACHINE_PROCESSING, MACHINE_BROKEN };
enum Case { NORMAL, BOTTLENECK };

typedef struct {
    std::string name;
    std::string stateName;
    int queueSize;
    int maxQueueSize;
    int outputNum;
    int processTime;
    int remainingTime;
    int health;
    int maxHealth;
    int progress;
    float breakdownChance;
} MachineData;

class Machine : public Root {
private:
    MachineState state;
    int processTime;
    std::shared_ptr<Product> currentProduct;
    int output_num;
    int remaining_time;
    std::weak_ptr<MachineListener> listener;
    std::shared_ptr<Machine> nextMachine;
    
protected:
    MachineState getState() const { return state; }
    void setState(MachineState s) { state = s; }
    int getProcessTime() const { return processTime; }
    void setProcessTime(int t) { processTime = t; }
    int getOutputNum() const { return output_num; }
    void setOutputNum(int n) { output_num = n; }
    std::shared_ptr<Product> getCurrentProduct() const { return currentProduct; }
    void setCurrentProduct(std::shared_ptr<Product> product) { currentProduct = product; }
    ProgressQueue queue;
    Durability durability;
    int normalProcessTime;
    int bottleneckProcessTime;
    void setCaseProcessTimes(int normalTime, int bottleneckTime) {
        normalProcessTime = normalTime;
        bottleneckProcessTime = bottleneckTime;
    }
    int getProcessTimeForCase(Case c) const;
    std::shared_ptr<Product> generateProduct(std::shared_ptr<Product> new_product);
    void handleBrokenState();
    void fetchNextProduct();
    template<typename T>
    void handleProcessing();
    std::shared_ptr<Machine> getNextMachine() const { return nextMachine; }
public:
    Machine(int processT, int repairT, float breakdownC);
    virtual ~Machine()=default;
    virtual void update(int tick)=0;
    virtual MachineData getInfo() const=0;
    virtual void switchCase(Case c);
    virtual void breakdown();
    void setNextMachine(std::shared_ptr<Machine> next) { nextMachine = next; }
    std::string getStateName() const;
    int getProgress() const;
    ProgressQueue& getQueue();
    void repair();
    void decreaseRemainingTime(int amount);
    bool isRemainTime() const;
    int getRemainingTime() const { return remaining_time; }
    void setListener(std::weak_ptr<MachineListener> newListener);
    void reset();
};

template <typename T>
void Machine::handleProcessing(){
    auto next=getNextMachine();
    decreaseRemainingTime(1);
    durability.decreaseHealth(1);
    if (!isRemainTime()) {
        if (!next) {
            std::shared_ptr<Product> done = getCurrentProduct();
            setCurrentProduct(nullptr);
            done->setState(DELETED);
            auto newWafer=std::make_shared<T>(*done);
            generateProduct(newWafer);
            setOutputNum(getOutputNum() + 1);
            if (durability.checkBreakdown()) {
                breakdown();
            }
        } else if (next && next->getQueue().getQueueSize() < next->getQueue().getMaxQueueSize()) {
            std::shared_ptr<Product> done = getCurrentProduct();
            auto newWafer=std::make_shared<T>(*done);
            std::shared_ptr<Product> processedProduct = generateProduct(newWafer);
            setCurrentProduct(nullptr);
            done->setState(DELETED);
            next->getQueue().addQueue(processedProduct);
            
            setOutputNum(getOutputNum() + 1);
            if (durability.checkBreakdown()) {
                breakdown();
            }
        } else {
            getCurrentProduct()->setState(DAMAGED);
            setCurrentProduct(nullptr);
            setState(MACHINE_IDLE);
        }
    }
}
#endif

