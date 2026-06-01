#include "Models/Generator.h"

Generator::Generator():generate_time(3), remain_time(3), onElementGenerated(nullptr) {}

void Generator::setGenerationCallback(void (*cb)(Root*)){
    onElementGenerated=cb;
}

void Generator::update(int tick){
    remain_time--;
    if (remain_time<=0){
        remain_time=generate_time;
        RawWafer* new_wafer=new RawWafer();
        if (onElementGenerated){
            onElementGenerated(new_wafer);
        }
    }
}