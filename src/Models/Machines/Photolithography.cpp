#include "Photolithography.h"
#include "../Products/PatternedWafer.h"
#include <memory>

std::shared_ptr<Product> Photolithography::makeWaferPtr(std::shared_ptr<Product> done) const {
    return std::make_shared<PatternedWafer>(*done);
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
    data.power=getPower();
    return data;
}

