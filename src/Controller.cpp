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

void Controller::onMachineBroken(const std::string& machineName){
    breakdown_num++;
    log(LOG_WARNING, machineName, "BREAKDOWN: machine failed. Technician dispatched for repair.");
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
    for (const auto& product : products){
        if (product) product->update(tick);
    }
    for (const auto& machine : machines){
        if (machine && machine->isPowered()) machine->update(tick);
    }
    products.erase(
        std::remove_if(products.begin(), products.end(), [this](const std::shared_ptr<Product>& p) {
        if (!p) return true;
        ProductState s = p->getState();
        if (s == DAMAGED || s == DELETED || s == COMPLETED) {
            // Report the product before it leaves the system. Products that reach a
            // terminal state during the machine-update phase would otherwise be erased
            // in the same tick, before their update() runs, so the loss/completion would
            // never be counted. Product::update() resets its listener after reporting,
            // so calling it again here cannot double-count already-counted products.
            p->update(tick);
            return true;
        }
        return false;
    }),
    products.end()
    );
    
    tick++;
}

MachineData Controller::getMachineInfo(int i) const{
    if (i>=0 && i<machines.size()){
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
    breakdown_num=0;
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
    const char* caseName = (c == BOTTLENECK) ? "BOTTLENECK" : (c == BREAKDOWN) ? "RANDOM BREAKDOWNS" : "NORMAL";
    log(LOG_WARNING, "System", "Factory Scenario updated to " + std::string(caseName));
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
        log(LOG_INFO, "System", "Instant Repair applied to machine " + std::to_string(index) + ".");
    }
}

void Controller::forceBreakMachine(int index) {
    if (index < 0 || index >= machines.size()) return;
    if (machines[index]) {
        machines[index]->breakdown();
    }
}

void Controller::setMachinePower(int index, bool on) {
    if (index < 0 || index >= machines.size()) return;
    if (machines[index]) {
        machines[index]->setPowered(on);
        log(LOG_INFO, "System", "Machine " + std::to_string(index) + " power " + (on ? "ON." : "OFF."));
    }
}