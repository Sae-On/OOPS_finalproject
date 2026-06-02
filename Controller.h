#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "Models/Root.h"
#include "Models/Machines/Machine.h"
#include "Models/Products/Product.h"
#include "Models/Generator.h"
#include <vector>

class Controller : public MachineListener, public ProductListener {
private:
    std::vector<Product*> products;
    std::vector<Machine*> machines;
    Generator* generator;
    std::vector<Product*> newly_generated_products;
    std::vector<Product*> products_to_delete;
    int tick=0;
    int lost_products_num=0;
    int completed_products_num=0;
    bool running=false;
    Case curr_case=NORMAL;
public:
    ~Controller();
    void addMachine(Machine* new_m);
    void setGenerator(Generator* new_generator);
    void onProductGenerated(Product* newProduct);
    void deleteProduct(Product* product, ProductState state);
    void updateAll();
    MachineData getMachineInfo(int i);
    void start();
    void pause();
    bool isRunning() const { return running; }
    void reset();
    void updateCase(Case c);
};

#endif