#include "Generator.h"

Generator::Generator():generate_time(4), remain_time(4), onElementGenerated(nullptr) {}

void Generator::setGenerationCallback(std::function<void(std::shared_ptr<Root>)> cb){
    onElementGenerated=std::move(cb);
}

void Generator::update(int tick){
    remain_time--;
    if (remain_time<=0){
        remain_time = generate_time;
        generate();
    }
}

void Generator::generate() {
    std::shared_ptr<RawWafer> new_wafer = std::make_shared<RawWafer>();
    if (onElementGenerated) {
        onElementGenerated(new_wafer);
    }
}

void Generator::reset() {
    remain_time = generate_time;
}