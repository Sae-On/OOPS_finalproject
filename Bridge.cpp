#include "Bridge.h"
#include "Initializer.h"

Bridge::Bridge() {
    controller = new Controller();
    Initializer initializer;
    initializer.Initialize(controller);
}

Bridge::~Bridge() {
    if (controller) {
        delete controller;
        controller = nullptr;
    }
}

std::string Bridge::getMachineName(int index) const {
    return controller->getMachineInfo(index).name;
}
std::string Bridge::getMachineState(int index) const {
    return controller->getMachineInfo(index).stateName;
}
int Bridge::getMachineHealth(int index) const {
    return controller->getMachineInfo(index).health;
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
int Bridge::getRawWaferCount() const {
    return controller->getMachineInfo(0).queueSize + (controller->getMachineInfo(0).stateName == "PROCESSING" ? 1 : 0);
}
int Bridge::getPatternedWaferCount() const {
    return controller->getMachineInfo(1).queueSize + (controller->getMachineInfo(1).stateName == "PROCESSING" ? 1 : 0);
}
int Bridge::getEtchedWaferCount() const {
    return controller->getMachineInfo(2).queueSize + (controller->getMachineInfo(2).stateName == "PROCESSING" ? 1 : 0);
}
int Bridge::getDopedWaferCount() const {
    return controller->getMachineInfo(3).queueSize + (controller->getMachineInfo(3).stateName == "PROCESSING" ? 1 : 0);
}
int Bridge::getFinishedCPUCount() const {
    return controller->getCompletedCount();
}
int Bridge::getDefectiveCount() const {
    return controller->getLostCount();
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
void Bridge::start() {
    controller->start();
}
void Bridge::pause() {
    controller->pause();
}
void Bridge::reset() {
    controller->reset();
}
