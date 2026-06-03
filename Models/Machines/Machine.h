#ifndef MACHINE_H
#define MACHINE_H
#include <string>
#include <vector>
#include "../Products/Product.h"
#include "../Root.h"
#include "../../ProgressQueue.h"
#include "Durability.h"
#include "MachineListener.h"

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
    int progress;
    float breakdownChance;
} MachineData;

class Machine : public Root {
private:
    MachineState state;
    int processTime;
    Product* currentProduct;
    int output_num;
    int remaining_time;
    MachineListener* listener = nullptr;
    Machine* nextMachine;
    
protected:
    MachineState getState() const { return state; }
    void setState(MachineState s) { state = s; }
    int getProcessTime() const { return processTime; }
    void setProcessTime(int t) { processTime = t; }
    int getOutputNum() const { return output_num; }
    void setOutputNum(int n) { output_num = n; }
    Product* getCurrentProduct() const { return currentProduct; }
    void setCurrentProduct(Product* product) { currentProduct = product; }
    ProgressQueue queue;
    Durability durability;
    int normalProcessTime;
    int bottleneckProcessTime;
    void setCaseProcessTimes(int normalTime, int bottleneckTime) {
        normalProcessTime = normalTime;
        bottleneckProcessTime = bottleneckTime;
    }
    int getProcessTimeForCase(Case c) const;
    Product* generateProduct(Product* new_product);
    void handleBrokenState();
    void fetchNextProduct();
    Machine* getNextMachine() const { return nextMachine; }
public:
    Machine(int processT, int repairT, float breakdownC);
    virtual ~Machine();
    virtual void update(int tick)=0;
    virtual MachineData getInfo() const=0;
    virtual void switchCase(Case c);
    virtual void breakdown();
    void setNextMachine(Machine* next) { nextMachine = next; }
    std::string getStateName() const;
    int getProgress() const;
    ProgressQueue& getQueue();
    void repair();
    void decreaseRemainingTime(int amount);
    bool isRemainTime() const;
    int getRemainingTime() const { return remaining_time; }
    void setListener(MachineListener* newListener);
    void reset();
};
#endif