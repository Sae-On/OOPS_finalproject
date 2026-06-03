#include "Controller.h"
#include <algorithm>

Controller::~Controller(){
    for (Product* p : products){
        delete p;
    }
    for (Machine* m : machines){
        delete m;
    }
    if (generator) {
        delete generator;
    }
    machines.clear();
    products.clear();
    for (Product* p : newly_generated_products){
        delete p;
    }
    newly_generated_products.clear();
    products_to_delete.clear();
}
void Controller::start() {
    running = true;
}
void Controller::pause() {
    running = false;
}
void Controller::addMachine(Machine* new_m){
    if (!new_m) return;
    new_m->setListener(this);
    machines.push_back(new_m);
}

void Controller::setGenerator(Generator* new_generator){
    if (!new_generator) return;
    generator = new_generator;
    generator->setGenerationCallback([this](Root* root){
        Product* product = dynamic_cast<Product*>(root);
        if (product) {
            this->onProductGenerated(product);
        } else {
            delete root;
        }
    });
}

void Controller::onProductGenerated(Product* newProduct){
    if (!newProduct) return;
    newProduct->setListener(this);
    if (newProduct->getType() == RAW && !machines.empty()) {
        machines.front()->getQueue().addQueue(newProduct);
    }
    newly_generated_products.push_back(newProduct);
}

void Controller::deleteProduct(Product* product, ProductState state){
    if (!product) return;
    auto check_it = std::find(products_to_delete.begin(), products_to_delete.end(), product);
    if (check_it != products_to_delete.end()) {
        return;
    }
    switch (state)
    {
    case DAMAGED:
        lost_products_num++;
        break;
    
    case COMPLETED:
        completed_products_num++;
        break;  
    default:
        break;
    }
    products_to_delete.push_back(product);
}

void Controller::update(){
    if (!running) return;
    if (generator) {
        generator->update(tick);
    }
    for (Product* p : newly_generated_products){
        if (p) {
            products.push_back(p);
        }
    }
    newly_generated_products.clear();
    for (Product* product : products){
        if (product) product->update(tick);
    }
    for (Machine* machine : machines){
        if (machine) machine->update(tick);
    }
    for (Product* dead_p : products_to_delete) {
        if (!dead_p) continue;
        auto it = std::find(products.begin(), products.end(), dead_p);
        if (it != products.end()) {
            products.erase(it);
        }
        delete dead_p; 
    }
    products_to_delete.clear();
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
    for (Product* p : products){
        delete p;
    }
    products.clear();
    for (Machine* m : machines){
        m->reset();
    }
    if (generator) {
        generator->reset();
    }
    for (Product* p : newly_generated_products){
        delete p;
    }
    newly_generated_products.clear();
    products_to_delete.clear();
}

void Controller::updateCase(Case c){
    if (curr_case == c) return;
    running=false;
    reset();
    curr_case=c;
    for (Machine* m : machines){
        if (m) m->switchCase(c);
    }
}

void Controller::addRawWafer(int count) {
    if (generator) {
        for (int i = 0; i < count; ++i) {
            generator->generate();
        }
    }
}

void Controller::repairMachine(int index) {
    if (index < 0 || index >= machines.size()) return;
    if (machines[index]) {
        machines[index]->repair();
    }
}