#ifndef MACHINE_H
#define MACHINE_H
#include <string>
#include <vector>
#include "Models/Products/Product.h"
#include "Models/Root.h"
#include "Models/Machines/Durability.h"
#include "ProgressQueue.h"

enum MachineState { MACHINE_IDLE, MACHINE_PROCESSING, MACHINE_BROKEN};
enum Case { NORMAL, BOTTLENECK};

class Machine : public Root {
private:
    MachineState state;
    int processTime;
    Product* currentProduct;
    int output_num;
    int remaining_time;

protected:
    int lost_product_num = 0;
    Machine* nextMachine = nullptr;

    ProgressQueue queue;
    Durability durability;

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

    virtual void update(int tick) = 0;
    virtual std::string getInfo() const = 0;
    virtual void switchCase(Case c) = 0;
    virtual void breakdown();
    virtual void repair();

    // 큐 관리 (public: 외부에서 제품 투입 및 상태 조회 필요)
    Product* popQueue();
    void addQueue(Product* product);
    int getQueueSize() const;
    bool hasCurrentProduct() const;
    int popLostProductNum();

    // nextMachine 연결 (public: FactoryController에서 체인 구성)
    void setNextMachine(Machine* next);
    Machine* getNextMachine() const;

    // 상태 조회 (UI 표시용 - public)
    std::string getStateName() const;
    int getProgress() const;
    int getMaxQueueSize() const;
    int getDisplayProcessTime() const;
    int getDisplayHealth() const;
    int getDisplayRemainingTime() const;
    int getDisplayOutputNum() const;
    float getDisplayBreakdownChance() const;
};
#endif

