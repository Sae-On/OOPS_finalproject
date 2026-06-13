#include "Bridge.h"
#include "Initializer.h"

Bridge::Bridge() {
    controller = std::make_unique<Controller>();
    Initializer initializer;
    initializer.Initialize(controller);
}

Bridge::~Bridge() {}

std::string Bridge::getMachineName(int index) const {
    return controller->getMachineInfo(index).name;
}
std::string Bridge::getMachineState(int index) const {
    return controller->getMachineInfo(index).stateName;
}
int Bridge::getMachineHealth(int index) const {
    auto info=controller->getMachineInfo(index);
    return (info.maxHealth!=0) ? (info.health*100)/info.maxHealth : 0;
}
int Bridge::getMachineProgress(int index) const {
    return controller->getMachineInfo(index).progress;
}
float Bridge::getMachineBreakdownChance(int index) const {
    return controller->getMachineInfo(index).breakdownChance;
}
int Bridge::getMachineProcessTime(int index) const {
    return controller->getMachineInfo(index).processTime;
}
int Bridge::getMachineRemainingTime(int index) const {
    return controller->getMachineInfo(index).remainingTime;
}
int Bridge::getMachineQueueSize(int index) const {
    return controller->getMachineInfo(index).queueSize;
}
int Bridge::getWaferCount(int index) const {
    return controller->getMachineInfo(index).queueSize + (controller->getMachineInfo(index).stateName == "PROCESSING" ? 1 : 0);
}
int Bridge::getFinishedCPUCount() const {
    return controller->getCompletedCount();
}
int Bridge::getDefectiveCount() const {
    return controller->getLostCount();
}
int Bridge::getBreakdownCount() const {
    return controller->getBreakdownCount();
}
int Bridge::getWipCount() const {
    return controller->getWipCount();
}
int Bridge::getTick() const {
    return controller->getTick();
}
void Bridge::updateCase(Case c) {
    controller->updateCase(c);
}
void Bridge::addRawWafer(int count) {
    controller->addRawWafer(count);
}
void Bridge::repairMachine(int index) {
    controller->repairMachine(index);
}
void Bridge::forceBreakMachine(int index) {
    controller->forceBreakMachine(index);
}
void Bridge::setMachinePower(int index, bool on) {
    controller->setMachinePower(index, on);
}
void Bridge::clearLog() {
    controller->clearLog();
}
void Bridge::start() {
    controller->start();
}
void Bridge::pause() {
    controller->pause();
}
void Bridge::reset() {
    controller->reset();
}
const std::vector<LogEntry>& Bridge::getLogEntries() const {
    return controller->getLogger().getLogs();
}