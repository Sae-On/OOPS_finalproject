#include "Etcher.h"
#include "../Products/EtchedWafer.h"

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
        decreaseRemainingTime(1);
        durability.decreaseHealth(1);
        if (!isRemainTime()) {
            if (getNextMachine() && getNextMachine()->getQueue().getQueueSize() < getNextMachine()->getQueue().getMaxQueueSize()) {
                Product* done = getCurrentProduct();
                Product* processedProduct = generateProduct(new EtchedWafer(*done));
                setCurrentProduct(nullptr);
                done->setState(DELETED);

                getNextMachine()->getQueue().addQueue(processedProduct);
                
                setOutputNum(getOutputNum() + 1);
                if (durability.checkBreakdown()) {
                    breakdown();
                }
            } else {
                getCurrentProduct()->setState(DAMAGED);
                setCurrentProduct(nullptr);
                setState(MACHINE_IDLE);
            }
        }
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
    data.health=durability.getHealth();
    data.progress=getProgress();
    return data;
}
