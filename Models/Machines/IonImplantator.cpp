#include "IonImplantator.h"
#include "../Products/DopedWafer.h"
#include <memory>

std::shared_ptr<Product> IonImplantator::makeWaferPtr(std::shared_ptr<Product> done) const {
    return std::make_shared<DopedWafer>(*done);
}

MachineData IonImplantator::getInfo() const {
    MachineData data;
    data.name="IonImplantator";
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
    data.power=getPower();
    return data;
}