#include "Initializer.h"
#include "Controller.h"
#include "Models/Machines/Etcher.h"
#include "Models/Machines/IonImplantator.h"
#include "Models/Machines/Packager.h"
#include "Models/Machines/Photolithography.h"

void Initializer::Initialize(Controller* controller) {
    Photolithography* photolithography = new Photolithography();
    Etcher* etcher = new Etcher();
    IonImplantator* ionImplantator = new IonImplantator();
    Packager* packager = new Packager();

    photolithography->setNextMachine(etcher);
    etcher->setNextMachine(ionImplantator);
    ionImplantator->setNextMachine(packager);

    controller->addMachine(photolithography);
    controller->addMachine(etcher);
    controller->addMachine(ionImplantator);
    controller->addMachine(packager);

    Generator* generator = new Generator();
    controller->setGenerator(generator);
}
