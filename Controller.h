#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "Models/Root.h"
#include "Models/Machines/Machine.h"
#include "Models/Products/Product.h"
#include "Models/Generator.h"
#include "Logger.h"
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
    Logger logger;
public:
    ~Controller();
    void addMachine(Machine* new_m);
    void setGenerator(Generator* new_generator);
    void onProductGenerated(Product* newProduct);
    void deleteProduct(Product* product, ProductState state);
    void update();
    MachineData getMachineInfo(int i) const;
    void start();
    void pause();
    bool isRunning() const { return running; }
    void reset();
    void updateCase(Case c);
    void addRawWafer(int count);
    void repairMachine(int index);
    int getCompletedCount() const { return completed_products_num; }
    int getLostCount() const { return lost_products_num; }
    void log(LogLevel level, const std::string& source, const std::string& message) {
        logger.addLog(tick, level, source, message);
    }
    const Logger& getLogger() const { return logger; }
    Logger& getLogger() { return logger; }
};

#endif