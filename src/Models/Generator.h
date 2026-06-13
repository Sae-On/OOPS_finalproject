#ifndef GENERATOR_H
#define GENERATOR_H
#include <functional>
#include "Root.h"
#include "Products/RawWafer.h"
#include <memory>
class Generator : public Root {
private:
    int generate_time;
    int remain_time;
    std::function<void(std::shared_ptr<Root>)> onElementGenerated;
public:
    Generator();
    void update(int tick) override;
    void setGenerationCallback(std::function<void(std::shared_ptr<Root>)> cb);
    void reset();
    void generate();
};
#endif