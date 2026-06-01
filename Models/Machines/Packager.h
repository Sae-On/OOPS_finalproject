#ifndef PACKAGER_H
#define PACKAGER_H
#include "Models/Machines/Machine.h"
#include <vector>

class Packager : public Machine {
private:
    // 최종 완성품 CPU를 보관하는 컨테이너
    std::vector<Product*> finishedProducts;
public:
    Packager() : Machine(10, 15, 0.15f) {}
    virtual ~Packager() override;

    virtual void update(int tick) override;
    virtual std::string getInfo() const override;
    virtual void switchCase(Case c) override;

    // 완성품 개수 조회
    int getFinishedCount() const;
    // 완성품 인출 (소유권 이전)
    std::vector<Product*> popFinishedProducts();
};
#endif