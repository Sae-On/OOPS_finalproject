#ifndef ROOT_H
#define ROOT_H
#include <string>

class Root {
    public:
        virtual ~Root()=default;
        virtual void update(int tick)=0;
};
#endif