#ifndef FACTORY_CONTROLLER_H
#define FACTORY_CONTROLLER_H

#include "Models/Machines/Photolithography.h"
#include "Models/Machines/Etcher.h"
#include "Models/Machines/IonImplantator.h"
#include "Models/Machines/Packager.h"
#include "Models/Products/RawWafer.h"
#include <string>
#include <vector>

// ─────────────────────────────────────────────────────────────
// FactoryController: 공정 체인 전체를 소유하고 제어하는 파사드 클래스
//   - UML의 BackendFacade 역할 수행
//   - 공정 순서: Photolithography -> Etcher -> IonImplantator -> Packager
// ─────────────────────────────────────────────────────────────
class FactoryController {
public:
    FactoryController();

    // ── 제어 인터페이스 ─────────────────────────────────────
    void start();                         // 라인 가동 시작
    void pause();                         // 라인 일시 정지
    void reset();                         // 전체 초기화

    void addRawWafer(int count);          // 원자재(RawWafer) 추가
    void repairMachine(int index);        // 지정 장비 수리
    void setPower(int index, bool on);    // 장비 전원 on/off

    // ── 틱(tick) 단위 시뮬레이션 업데이트 ──────────────────
    void update();

    // ── 상태 조회 (UI 데이터 제공) ──────────────────────────
    bool isRunning() const;

    // 장비 정보 (인덱스: 0=Photolithography, 1=Etcher, 2=IonImplantator, 3=Packager)
    std::string getMachineName(int index) const;
    std::string getMachineState(int index) const;
    int getMachineHealth(int index) const;
    int getMachineProgress(int index) const;
    float getMachineBreakdownChance(int index) const;
    int getMachineProcessTime(int index) const;
    int getMachineRemainingTime(int index) const;
    int getMachineQueueSize(int index) const;
    int getMachineOutputNum(int index) const;
    bool getMachinePower(int index) const;

    // 제품 인벤토리 개수
    int getRawWaferCount() const;
    int getPatternedWaferCount() const;
    int getEtchedWaferCount() const;
    int getDopedWaferCount() const;
    int getFinishedCPUCount() const;
    int getDefectiveCount() const;

private:
    bool running_ = false;

    // 공정 체인 장비들 (소유권 보유)
    Photolithography photolithography_;
    Etcher           etcher_;
    IonImplantator   ionImplantator_;
    Packager         packager_;

    // 전원 상태 (전원 off 시 update 건너뜀)
    bool power_[4] = {true, true, true, true};

    // 재료 재고
    int rawWaferCount_    = 0;

    // 완성/불량 집계
    int finishedCPUCount_ = 0;
    int defectiveCount_   = 0;

    // 내부 도우미
    Machine* getMachine(int index);
    const Machine* getMachine(int index) const;
    void collectFinishedProducts();
};

#endif
