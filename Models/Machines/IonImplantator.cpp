#include "IonImplantator.h"
#include "../Products/DopedWafer.h"

void IonImplantator::update(int tick) {
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
                Product* processedProduct = generateProduct(new DopedWafer(*done));
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

MachineData IonImplantator::getInfo() const {
    MachineData data;
    data.name="IonImplantator";
    data.stateName=getStateName();
    data.queueSize=queue.getQueueSize();
    data.maxQueueSize=queue.getMaxQueueSize();
    data.outputNum=getOutputNum();
    data.processTime=getProcessTime();
    data.health=durability.getHealth();
    data.progress=getProgress();
    return data;
}