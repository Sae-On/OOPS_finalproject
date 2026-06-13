#include "Etcher.h"
#include "../Products/EtchedWafer.h"
#include <memory>

void Etcher::update(int tick) {
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
        handleProcessing<EtchedWafer>();
    }  
}

MachineData Etcher::getInfo() const {
    MachineData data;
    data.name="Etcher";
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
