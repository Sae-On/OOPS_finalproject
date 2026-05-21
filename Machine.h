#ifndef MACHINE_H
#define MACHINE_H
#include <string>
#include <vector>
#include "Product.h"
#include "Root.h"

class Machine : public Root {
public:
    static const int STATE_IDLE = 0;
    static const int STATE_PROCESSING = 1;
    static const int STATE_BROKEN = 2;
    static const int CASE_NORMAL = 0;
    static const int CASE_BOTTLENECK = 1;

private:
    int state;
    int processTime;
    int health;
    std::vector<Product*> queue;
    Product* currentProduct;
    int output_num;
    int remaining_time;
    int repair_time;
    float breakdown_chance;
protected:
    int getState() const;
    void setState(int s);
    int getProcessTime() const;
    void setProcessTime(int t);
    int getHealth() const;
    void setHealth(int h);
    int getOutputNum() const;
    void setOutputNum(int n);
    int getRemainingTime() const;
    void setRemainingTime(int t);
    int getRepairTime() const;
    void setRepairTime(int t);
    float getBreakdownChance() const;
    void setBreakdownChance(float c);
    Product* getCurrentProduct() const;
    void setCurrentProduct(Product* product);
public:
    Machine(int processT, int repairT, float breakdownC);
    virtual ~Machine();
    virtual void update(int tick)=0;
    virtual std::string getInfo() const=0;
    virtual void switchCase(int caseNum)=0;
    virtual void breakdown();
    virtual void repair();
    Product* popQueue();
    void addQueue(Product* product);
};
#endif