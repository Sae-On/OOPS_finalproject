#include "Packager.h"
#include "../Products/CPU.h"

void Packager::update(int tick) {
    if (getState() == MACHINE_BROKEN) {
        handleBrokenState();
        return;
    }

    if (getCurrentProduct() == nullptr) {
        fetchNextProduct();
        if (getCurrentProduct()==nullptr){
            return;
        }
    }

    if (getCurrentProduct() != nullptr) {
        handleProcessing<CPU>();
    }
}

MachineData Packager::getInfo() const {
    MachineData data;
    data.name="Packager";
    data.stateName=getStateName();
    data.queueSize=queue.getQueueSize();
    data.maxQueueSize=queue.getMaxQueueSize();
    data.outputNum=getOutputNum();
    data.processTime=getProcessTime();
    data.remainingTime=getRemainingTime();
    data.health=durability.getHealth();
    data.maxHealth=durability.getMaxHealth();
    data.progress=getProgress();
    data.breakdownChance=durability.getBreakdownChance();
    return data;
}
