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
        decreaseRemainingTime(1);
        durability.decreaseHealth(1);
        if (!isRemainTime()) {
            Product* done = getCurrentProduct();
            setCurrentProduct(nullptr);
            done->setState(DELETED);
            setOutputNum(getOutputNum() + 1);
            if (durability.checkBreakdown()) {
                breakdown();
            }
        }
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
    data.health=durability.getHealth();
    data.progress=getProgress();
    return data;
}
