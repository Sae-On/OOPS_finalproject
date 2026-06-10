#ifndef INITIALIZER_H
#define INITIALIZER_H
#include <memory>

class Controller;

class Initializer {
public:
    void Initialize(std::shared_ptr<Controller>& controller);
};

#endif
