#ifndef BRIDGE_H
#define BRIDGE_H
#include "Controller.h"
class Bridge {
private:
    Controller* controller;
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
    int getRawWaferCount() const;
    int getPatternedWaferCount() const;
    int getEtchedWaferCount() const;
    int getDopedWaferCount() const;
    int getFinishedCPUCount() const;
    int getDefectiveCount() const;
    void updateCase(Case c);
    void addRawWafer(int count);
    void repairMachine(int index);
    void start();
    void pause();
    bool isRunning() const { return controller->isRunning(); }
    void reset();
    void update() { controller->update(); }
    const std::vector<LogEntry>& getLogEntries() const;
};
#endif