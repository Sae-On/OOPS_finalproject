#include "Controller.h"
#include <algorithm>

Controller::~Controller(){
}
void Controller::start() {
    running = true;
}
void Controller::pause() {
    running = false;
}
void Controller::addMachine(std::shared_ptr<Machine> new_m){
    if (!new_m) return;
    new_m->setListener(shared_from_this());
    machines.push_back(new_m);
}

void Controller::setGenerator(std::unique_ptr<Generator> new_generator){
    if (!new_generator) return;
    generator = std::move(new_generator);
    generator->setGenerationCallback([this](std::shared_ptr<Root> root){
        auto product = std::dynamic_pointer_cast<Product>(root);
        if (product) {
            this->onProductGenerated(product);
        }
    });
}

void Controller::onProductGenerated(std::shared_ptr<Product> newProduct){
    if (!newProduct) return;
    newProduct->setListener(shared_from_this());
    if (newProduct->getType() == RAW && !machines.empty()) {
        machines.front()->getQueue().addQueue(newProduct);
    }
    newly_generated_products.push_back(newProduct);
}

void Controller::onMachineBreakdown(){
    totalBreakdownCount++;
}

void Controller::deleteProduct(std::shared_ptr<Product> product, ProductState state){
    if (!product) return;
    switch (state)
    {
    case DAMAGED:
        lost_products_num++;
        log(LOG_ERROR, "System", "DEFECT: Product DAMAGED and safely disposed.");
        break;
    
    case COMPLETED:
        completed_products_num++;
        log(LOG_INFO, "System", "SUCCESS: CPU completed and packaged for shipping! Total: " + std::to_string(completed_products_num));
        break;  
    default:
        break;
    }
}

void Controller::update(){
    if (!running) return;
    if (generator) {
        generator->update(tick);
    }
    for (const auto& p : newly_generated_products){
        if (p) {
            products.push_back(p);
        }
    }
    newly_generated_products.clear();
    for (const auto& machine : machines){
        if (machine) machine->update(tick);
    }
    for (const auto& product : products){
        if (product) product->update(tick);
    }
    products.erase(
        std::remove_if(products.begin(), products.end(), [](const std::shared_ptr<Product>& p) {
        if (!p) return true;
        ProductState s = p->getState();
        return (s == DAMAGED || s == DELETED || s == COMPLETED);
    }),
    products.end()
    );
    
    tick++;
}

MachineData Controller::getMachineInfo(int i) const{
    if (i>=0 and i<machines.size()){
        if (!machines[i]) return {"UNKNOWN","UNKNOWN",0,0,0,0,0,0,0};
        return machines[i]->getInfo();
    } else {
        return {"UNKNOWN","UNKNOWN",0,0,0,0,0,0,0};
    }
}

void Controller::reset(){
    tick=0;
    lost_products_num=0;
    completed_products_num=0;
    totalBreakdownCount=0;
    products.clear();
    for (const auto& m : machines){
        m->reset();
    }
    if (generator) {
        generator->reset();
    }
    newly_generated_products.clear();
}

void Controller::updateCase(Case c){
    if (curr_case == c) return;
    running=false;
    reset();
    curr_case=c;
    for (const auto& m : machines){
        if (m) m->switchCase(c);
    }
    log(LOG_WARNING, "System", "Factory Scenario updated to " + std::string(c == BOTTLENECK ? "BOTTLENECK" : "NORMAL"));
}

void Controller::addRawWafer(int count) {
    if (generator) {
        for (int i = 0; i < count; ++i) {
            generator->generate();
        }
    }
    log(LOG_INFO, "Generator", std::to_string(count) + " RawWafers queued by User.");
}

void Controller::repairMachine(int index) {
    if (index < 0 || index >= machines.size()) return;
    if (machines[index]) {
        machines[index]->repair();
    }
}

void Controller::forceBreakMachine(int index) {
    if (index < 0 || index >= machines.size()) return;
    if (machines[index]) {
        machines[index]->breakdown();
    }
}

void Controller::setPower(int index, bool power) {
    if (index < 0 || index >= machines.size()) return;
    if (machines[index]) {
        machines[index]->setPower(power);
    }
}