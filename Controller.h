#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "Models/Root.h"
#include "Models/Machines/Machine.h"
#include "Models/Products/Product.h"
#include "Models/Generator.h"
#include "Logger.h"
#include <vector>

class Controller : public MachineListener, public ProductListener, public std::enable_shared_from_this<Controller> {
private:
    std::vector<std::shared_ptr<Product>> products;
    std::vector<std::shared_ptr<Machine>> machines;
    std::unique_ptr<Generator> generator;
    std::vector<std::shared_ptr<Product>> newly_generated_products;
    int tick=0;
    int lost_products_num=0;
    int completed_products_num=0;
    int totalBreakdownCount=0;
    bool running=false;
    Case curr_case=NORMAL;
    Logger logger;
public:
    ~Controller();
    void addMachine(std::shared_ptr<Machine> new_m);
    void setGenerator(std::unique_ptr<Generator> new_generator);
    void onProductGenerated(std::shared_ptr<Product> newProduct);
    void onMachineBreakdown();
    void deleteProduct(std::shared_ptr<Product> product, ProductState state);
    void update();
    MachineData getMachineInfo(int i) const;
    void start();
    void pause();
    bool isRunning() const { return running; }
    void reset();
    void updateCase(Case c);
    void addRawWafer(int count);
    void repairMachine(int index);
    void forceBreakMachine(int index);
    int getCompletedCount() const { return completed_products_num; }
    int getLostCount() const { return lost_products_num; }
    int getTotalBreakdownCount() const { return totalBreakdownCount; }
    int getWipCount() const { return static_cast<int>(products.size()); }
    int getTick() const {return tick;}
    void log(LogLevel level, const std::string& source, const std::string& message) {
        logger.addLog(tick, level, source, message);
    }
    Logger& getLogger() { return logger; }
    void setPower(int index, bool power);
};

#endif