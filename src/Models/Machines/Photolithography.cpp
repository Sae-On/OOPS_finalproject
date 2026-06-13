#include "Photolithography.h"
#include "../Products/PatternedWafer.h"
#include <memory>

void Photolithography::update(int tick) {
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

    auto next=getNextMachine();
    if (getCurrentProduct() != nullptr) {
        handleProcessing<PatternedWafer>();
    }
    
}

MachineData Photolithography::getInfo() const {
    MachineData data;
    data.name="Photolithography";
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

