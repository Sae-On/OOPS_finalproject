#include "Generator.h"

Generator::Generator():generate_time(3), remain_time(3), onElementGenerated(nullptr) {}

void Generator::setGenerationCallback(std::function<void(Root*)> cb){
    onElementGenerated=std::move(cb);
}

void Generator::update(int tick){
    remain_time--;
    if (remain_time<=0){
        remain_time=generate_time;
        RawWafer* new_wafer=new RawWafer();
        if (onElementGenerated){
            onElementGenerated(new_wafer);
        } else {
            delete new_wafer;
        }
    }
}

void Generator::reset() {
    remain_time = generate_time;
}