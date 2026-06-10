#include "Initializer.h"
#include "Controller.h"
#include "Models/Machines/Etcher.h"
#include "Models/Machines/IonImplantator.h"
#include "Models/Machines/Packager.h"
#include "Models/Machines/Photolithography.h"

void Initializer::Initialize(std::shared_ptr<Controller>& controller) {
    auto photolithography = std::make_shared<Photolithography>();
    auto etcher = std::make_shared<Etcher>();
    auto ionImplantator = std::make_shared<IonImplantator>();
    auto packager = std::make_shared<Packager>();

    photolithography->setNextMachine(etcher);
    etcher->setNextMachine(ionImplantator);
    ionImplantator->setNextMachine(packager);

    controller->addMachine(photolithography);
    controller->addMachine(etcher);
    controller->addMachine(ionImplantator);
    controller->addMachine(packager);

    auto generator = std::make_unique<Generator>();
    controller->setGenerator(std::move(generator));
}
