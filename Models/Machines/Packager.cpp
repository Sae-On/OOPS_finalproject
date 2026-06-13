#include "Packager.h"
#include "../Products/CPU.h"


std::shared_ptr<Product> Packager::makeWaferPtr(std::shared_ptr<Product> done) const {
    return std::make_shared<CPU>(*done);
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
    data.power=getPower();
    return data;
}
