#ifndef ROOT_H
#define ROOT_H
#include <string>

class Root {
    public:
        Root();
        virtual ~Root();
        virtual void update(int tick)=0;
};
#endif