#include "Etcher.h"
#include "../Products/EtchedWafer.h"
#include <memory>

std::shared_ptr<Product> Etcher::makeWaferPtr(std::shared_ptr<Product> done) const {
    return std::make_shared<EtchedWafer>(*done);
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
    data.power=getPower();
    return data;
}
