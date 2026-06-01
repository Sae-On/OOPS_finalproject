#ifndef MACHINE_H
#define MACHINE_H
#include <string>
#include <vector>
#include "Models/Products/Product.h"
#include "Models/Root.h"
#include "ProgressQueue.h"
#include "Durability.h"
#include "MachineListener.h"

enum MachineState { IDLE, PROCESSING, BROKEN };
enum Case { NORMAL, BOTTLENECK};

class Machine : public Root {
private:
    MachineState state;
    int processTime;
    Product* currentProduct;
    int output_num;
    int remaining_time;
    MachineListener* listener = nullptr;

    
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
    Product* generateProduct(Product* new_product);
    void handleBrokenState();
    void fetchNextProduct();
public:
    Machine(int processT, int repairT, float breakdownC);
    virtual ~Machine();
    virtual void update(int tick)=0;
    virtual std::string getInfo() const=0;
    virtual void switchCase(Case c)=0;
    virtual void breakdown();
    std::string getStateName() const;
    int getProgress() const;
    ProgressQueue& getQueue();
    void repair();
    void decreaseRemainingTime(int amount);
    bool isRemainTime() const;
    void setListener(MachineListener* newListener);
};
#endif