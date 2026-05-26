#ifndef MACHINE_H
#define MACHINE_H
#include <string>
#include <vector>
#include "Models/Products/Product.h"
#include "Models/Root.h"

enum MachineState { IDLE, PROCESSING, BROKEN};
enum Case { NORMAL, BOTTLENECK};

class Machine : public Root {
private:
    MachineState state;
    int processTime;
    int health;
    std::vector<Product*> queue;
    Product* currentProduct;
    int output_num;
    int remaining_time;
    int repair_time;
    float breakdown_chance;
    int maxQueueSize = 6;
protected:
    MachineState getState() const;
    void setState(MachineState s);
    int getProcessTime() const;
    void setProcessTime(int t);
    int getHealth() const;
    void setHealth(int h);
    int getOutputNum() const;
    void setOutputNum(int n);
    int getRemainingTime() const;
    void setRemainingTime(int t);
    int getRepairTime() const;
    float getBreakdownChance() const;
    Product* getCurrentProduct() const;
    void setCurrentProduct(Product* product);
public:
    Machine(int processT, int repairT, float breakdownC);
    virtual ~Machine();
    virtual void update(int tick)=0;
    virtual std::string getInfo() const=0;
    virtual void switchCase(Case c)=0;
    virtual void breakdown();
    virtual void repair();
    Product* popQueue();
    void addQueue(Product* product);
    int getQueueSize() const;
    std::string getStateName() const;
    int getProgress() const;
    int getMaxQueueSize() const;
};
#endif