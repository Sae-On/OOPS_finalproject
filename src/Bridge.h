#ifndef BRIDGE_H
#define BRIDGE_H
#include "Controller.h"
class Bridge {
private:
    std::shared_ptr<Controller> controller;
public:
    Bridge();
    ~Bridge();
    std::string getMachineName(int index)  const;
    std::string getMachineState(int index) const;
    int getMachineHealth(int index) const;
    int getMachineProgress(int index) const;
    float getMachineBreakdownChance(int index) const;
    int getMachineProcessTime(int index) const;
    int getMachineRemainingTime(int index) const;
    int getMachineQueueSize(int index) const;
    int getWaferCount(int index) const;
    bool getMachinePower(int index) const;
    int getFinishedCPUCount() const;
    int getDefectiveCount() const;
    int getWipCount() const;
    bool isRunning() const { return controller->isRunning(); }
    std::vector<LogEntry> getLogEntries() const;
    int getTotalBreakdownCount() const;
    int getTick() const;
    void updateCase(Case c);
    void addRawWafer(int count);
    void repairMachine(int index);
    void forceBreakMachine(int index);
    void setPower(int index, bool power);
    void start();
    void pause();
    void reset();
    void update() { controller->update(); }
    void clearLog();
};
#endif