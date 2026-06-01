#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "Models/Root.h"
#include "Models/Machines/Machine.h"
#include "Models/Products/Product.h"
#include <vector>

class Controller : public MachineListener, public ProductListener {
private:
    std::vector<Root*> models;
    std::vector<Machine*> machines;
    std::vector<Product*> newly_generated_products;
    std::vector<Product*> products_to_delete;
    int tick;
    int lost_products_num=0;
    int completed_products_num=0;
public:
    ~Controller();
    void addMachine(Machine* new_m);
    void onProductGenerated(Product* newProduct);
    void deleteProduct(Product* product, ProductState state);
    void updateAll();
};

#endif