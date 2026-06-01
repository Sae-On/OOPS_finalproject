#ifndef MACHINE_H
#define MACHINE_H
#include <string>
#include <vector>
#include "Models/Products/Product.h"
#include "Models/Root.h"

enum MachineState { MACHINE_IDLE, MACHINE_PROCESSING, MACHINE_BROKEN};
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
    // nextMachine을 기반 클래스로 이동: 하위 클래스의 코드 중복 제거
    Machine* nextMachine = nullptr;

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
