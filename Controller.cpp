#include "Controller.h"
#include <algorithm>

Controller::~Controller(){
    for (Root* m : models){
        delete m;
    }
    machines.clear();
    models.clear();
    for (Product* p : newly_generated_products){
        delete p;
    }
    newly_generated_products.clear();
    products_to_delete.clear();
}
void Controller::addMachine(Machine* new_m){
    if (!new_m) return;
    new_m->setListener(this);
    machines.push_back(new_m);
    models.push_back(new_m);
}

void Controller::onProductGenerated(Product* newProduct){
    if (!newProduct) return;
    newly_generated_products.push_back(newProduct);
}

void Controller::deleteProduct(Product* product, ProductState state){
    if (!product) return;
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

void Controller::updateAll(){
    for (Root* p : newly_generated_products){
        models.push_back(p);
    }
    newly_generated_products.clear();
    for (Root* model : models){
        if (model) model->update(tick);
    }
    for (Product* dead_p : products_to_delete) {
        if (!dead_p) continue;
        auto it = std::find(models.begin(), models.end(), dead_p);
        if (it != models.end()) {
            models.erase(it);
        }
        delete dead_p; 
    }
    products_to_delete.clear();
    tick++;
}