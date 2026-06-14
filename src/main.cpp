#define GL_SILENCE_DEPRECATION
#define SDL_MAIN_HANDLED   // we provide our own main(); do not let SDL redefine it

#include <algorithm>
#include <array>
#include <cstdio>
#include <string>
#include <vector>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include <SDL.h>
#include <SDL_opengl.h>

#include "Bridge.h"

// ─────────────────────────────────────────────────────────────
// View Data: 백엔드 -> UI 전달용 데이터 구조체
// ─────────────────────────────────────────────────────────────

enum class ProductKind {
    RawWafer, PatternedWafer, EtchedWafer, DopedWafer, CPU
};

enum class WorkState {
    Idle, Processing, Broken
};

struct ControlPanelRequest {
    bool requestStart          = false;
    bool requestPause          = false;
    bool requestReset          = false;
    bool requestAddRawWafer    = false;
    bool requestRepair         = false;
    bool requestForceBreak     = false;
    bool requestClearLog       = false;
    bool requestPowerToggle    = false;
    bool requestScenarioChange = false;
    int  selectedMachineIndex  = 0;
    int  rawWaferAmount        = 5;
    int  simulationSpeed       = 1;   // 1x - 5x ticks
    int  selectedScenario      = 0;   // 0 Normal, 1 Bottleneck
    std::array<bool, 4> machinePowerOn = {true, true, true, true};
};

struct MachineStatusViewData {
    std::string name;
    std::string inputProduct;
    std::string outputProduct;
    float progress      = 0.0f;
    float durability    = 100.0f;
    float defectRate    = 0.0f;
    float processTime   = 0.0f;
    float remainingTime = 0.0f;
    int   waitingCount  = 0;
    WorkState state     = WorkState::Idle;
    bool  powerOn       = true;
};

struct ProductStatusViewData {
    ProductKind kind    = ProductKind::RawWafer;
    std::string name;
    int  count          = 0;
    int  defectiveCount = 0;
    ImVec4 color;
};

struct ViewportViewData {
    int activeMachineIndex = 0;
    int activeProductIndex = 0;
    int tick               = 0;
    int finishedGoods      = 0;
    int wipCount           = 0;
    int totalBreakdowns    = 0;
    int lostProducts       = 0;
    std::array<MachineStatusViewData, 4> machines;
    std::array<ProductStatusViewData, 5> products;
};

// ─────────────────────────────────────────────────────────────
// FactoryController -> ViewportViewData 변환
// ─────────────────────────────────────────────────────────────

static WorkState StateFromString(const std::string& s) {
    if (s == "PROCESSING") return WorkState::Processing;
    if (s == "BROKEN")     return WorkState::Broken;
    return WorkState::Idle;
}

static ViewportViewData BuildViewData(const Bridge& bridge, int selectedMachine) {
    static const char* kInputs[]  = {"RawWafer",      "PatternedWafer", "EtchedWafer",  "DopedWafer"};
    static const char* kOutputs[] = {"PatternedWafer", "EtchedWafer",   "DopedWafer",   "CPU"};

    ViewportViewData vd;
    vd.activeMachineIndex = selectedMachine;
    vd.activeProductIndex = std::clamp(selectedMachine, 0, 4);
    vd.tick            = bridge.getTick();
    vd.finishedGoods   = bridge.getFinishedCPUCount();
    vd.wipCount        = bridge.getWipCount();
    vd.totalBreakdowns = bridge.getTotalBreakdownCount();
    vd.lostProducts    = bridge.getDefectiveCount();

    for (int i = 0; i < 4; ++i) {
        MachineStatusViewData& m = vd.machines[i];
        m.name          = bridge.getMachineName(i);
        m.inputProduct  = kInputs[i];
        m.outputProduct = kOutputs[i];
        m.progress      = bridge.getMachineProgress(i) / 100.0f;
        m.durability    = static_cast<float>(bridge.getMachineHealth(i));
        m.defectRate    = bridge.getMachineBreakdownChance(i) * 100.0f;
        m.processTime   = static_cast<float>(bridge.getMachineProcessTime(i));
        m.remainingTime = static_cast<float>(bridge.getMachineRemainingTime(i));
        m.waitingCount  = bridge.getMachineQueueSize(i);
        m.state         = StateFromString(bridge.getMachineState(i));
    }

    static const ImVec4 kColors[5] = {
        {0.72f, 0.80f, 0.86f, 1.0f},
        {0.64f, 0.53f, 0.84f, 1.0f},
        {0.88f, 0.56f, 0.45f, 1.0f},
        {0.88f, 0.72f, 0.36f, 1.0f},
        {0.45f, 0.74f, 0.58f, 1.0f},
    };
    static const char* kProductNames[5] = {
        "RawWafer", "PatternedWafer", "EtchedWafer", "DopedWafer", "CPU"
    };
    static const ProductKind kKinds[5] = {
        ProductKind::RawWafer, ProductKind::PatternedWafer,
        ProductKind::EtchedWafer, ProductKind::DopedWafer, ProductKind::CPU
    };
    static const int kCounts[5] = {0}; // placeholder per-frame computed below

    vd.products[0] = {ProductKind::RawWafer,      "RawWafer",      bridge.getWaferCount(0),       0, kColors[0]};
    vd.products[1] = {ProductKind::PatternedWafer, "PatternedWafer",bridge.getWaferCount(1), 0, kColors[1]};
    vd.products[2] = {ProductKind::EtchedWafer,    "EtchedWafer",   bridge.getWaferCount(2),    0, kColors[2]};
    vd.products[3] = {ProductKind::DopedWafer,     "DopedWafer",    bridge.getWaferCount(3),     0, kColors[3]};
    vd.products[4] = {ProductKind::CPU,            "CPU",           bridge.getFinishedCPUCount(),    bridge.getDefectiveCount(), kColors[4]};

    return vd;
}

// ─────────────────────────────────────────────────────────────
// 드로잉 유틸리티
// ─────────────────────────────────────────────────────────────

static ImU32 ToColor(const ImVec4& color) {
    return ImGui::ColorConvertFloat4ToU32(color);
}

static ImU32 DimColor(const ImVec4& color, float factor) {
    return ImGui::ColorConvertFloat4ToU32(ImVec4(
        std::clamp(color.x * factor, 0.0f, 1.0f),
        std::clamp(color.y * factor, 0.0f, 1.0f),
        std::clamp(color.z * factor, 0.0f, 1.0f),
        color.w
    ));
}

static ImVec4 StateColor(WorkState state) {
    if (state == WorkState::Processing) return ImVec4(0.24f, 0.78f, 0.42f, 1.0f);
    if (state == WorkState::Broken)     return ImVec4(0.92f, 0.22f, 0.22f, 1.0f);
    return ImVec4(0.50f, 0.54f, 0.60f, 1.0f);
}

static const char* StateLabel(WorkState state) {
    if (state == WorkState::Processing) return "Processing";
    if (state == WorkState::Broken)     return "Broken";
    return "Idle";
}

static void DrawChip(ImDrawList* draw, ImVec2 center, float size) {
    const float half = size * 0.5f;
    ImVec2 min(center.x - half, center.y - half);
    ImVec2 max(center.x + half, center.y + half);
    ImU32 pin = IM_COL32(218, 224, 229, 255);

    for (int i = 0; i < 6; ++i) {
        float y = min.y + 8.0f + i * (size - 16.0f) / 5.0f;
        float x = min.x + 8.0f + i * (size - 16.0f) / 5.0f;
        draw->AddLine(ImVec2(min.x - 8.0f, y), ImVec2(min.x, y), pin, 2.0f);
        draw->AddLine(ImVec2(max.x, y), ImVec2(max.x + 8.0f, y), pin, 2.0f);
        draw->AddLine(ImVec2(x, min.y - 8.0f), ImVec2(x, min.y), pin, 2.0f);
        draw->AddLine(ImVec2(x, max.y), ImVec2(x, max.y + 8.0f), pin, 2.0f);
    }

    draw->AddRectFilled(min, max, IM_COL32(46, 98, 120, 255), 6.0f);
    draw->AddRect(min, max, IM_COL32(255, 255, 255, 150), 6.0f, 0, 2.0f);
    draw->AddRectFilled(ImVec2(min.x + 14.0f, min.y + 14.0f), ImVec2(max.x - 14.0f, max.y - 14.0f), IM_COL32(30, 38, 48, 255), 4.0f);
    draw->AddText(ImVec2(center.x - 16.0f, center.y - 7.0f), IM_COL32(240, 248, 255, 255), "CPU");
}

static void DrawWafer(ImDrawList* draw, ImVec2 center, float radius, ImU32 dieColor) {
    draw->AddCircleFilled(center, radius, IM_COL32(196, 215, 228, 255), 48);
    draw->AddCircle(center, radius, IM_COL32(255, 255, 255, 180), 48, 2.0f);
    draw->AddCircle(center, radius * 0.32f, IM_COL32(112, 140, 158, 120), 32, 1.0f);

    const float cell = radius * 0.28f;
    for (int y = -3; y <= 3; ++y) {
        for (int x = -3; x <= 3; ++x) {
            float dx = static_cast<float>(x) * cell;
            float dy = static_cast<float>(y) * cell;
            if (dx * dx + dy * dy > radius * radius * 0.62f) continue;
            ImVec2 p(center.x + dx, center.y + dy);
            draw->AddRectFilled(ImVec2(p.x - cell * 0.36f, p.y - cell * 0.36f),
                                ImVec2(p.x + cell * 0.36f, p.y + cell * 0.36f),
                                dieColor, 2.0f);
        }
    }
}

// ─────────────────────────────────────────────────────────────
// UI 윈도우 클래스들
// ─────────────────────────────────────────────────────────────

class ControlPanelWindow {
public:
    void Render(ControlPanelRequest& request, const ViewportViewData& viewportData, bool isRunning) {
        ImGui::SetNextWindowPos(ImVec2(16, 16), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(350, 410), ImGuiCond_FirstUseEver);
        ImGui::Begin("Control Panel");

        // ── 라인 제어 ──
        if (ImGui::Button(isRunning ? "Pause" : "Start", ImVec2(104, 32))) {
            if (isRunning) request.requestPause = true;
            else           request.requestStart = true;
            AddLog(isRunning ? "send: FactoryController::pause()" : "send: FactoryController::start()");
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset", ImVec2(104, 32))) {
            request.requestReset = true;
            AddLog("send: FactoryController::reset()");
        }

        // ── 라이브 틱 카운터 & 속도 ──
        ImGui::Text("Tick: %d   %s", viewportData.tick, isRunning ? "(running)" : "(paused)");
        if (ImGui::SliderInt("Speed", &request.simulationSpeed, 1, 5, "%dx")) {
            AddLog("set: simulation speed = " + std::to_string(request.simulationSpeed) + "x");
        }

        // ── 시나리오 선택 ──
        ImGui::Separator();
        const char* scenarioNames[] = {"Normal flow", "Bottleneck"};
        if (ImGui::Combo("Scenario", &request.selectedScenario, scenarioNames, 2)) {
            request.requestScenarioChange = true;
            AddLog(std::string("send: FactoryController::updateCase(") + scenarioNames[request.selectedScenario] + ")");
        }

        // ── 재료 추가 ──
        ImGui::Separator();
        ImGui::InputInt("RawWafer amount", &request.rawWaferAmount);
        request.rawWaferAmount = std::max(1, request.rawWaferAmount);
        if (ImGui::Button("Add RawWafer", ImVec2(-1.0f, 30.0f))) {
            request.requestAddRawWafer = true;
            AddLog("send: FactoryController::addRawWafer(" + std::to_string(request.rawWaferAmount) + ")");
        }

        // ── 장비 제어 ──
        ImGui::Separator();
        const char* machineNames[] = {"Photolithography", "Etcher", "IonImplantator", "Packager"};
        ImGui::Combo("Target machine", &request.selectedMachineIndex, machineNames, 4);

        bool& pw = request.machinePowerOn[request.selectedMachineIndex];
        if (ImGui::Checkbox("Power on", &pw)) {
            request.requestPowerToggle = true;
            AddLog(std::string("send: FactoryController::setPower(") + (pw ? "true)" : "false)"));
        }
        float halfW = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;
        if (ImGui::Button("Force Break", ImVec2(halfW, 30.0f))) {
            request.requestForceBreak = true;
            AddLog("send: FactoryController::forceBreakMachine()");
        }
        ImGui::SameLine();
        if (ImGui::Button("Instant Repair", ImVec2(halfW, 30.0f))) {
            request.requestRepair = true;
            AddLog("send: FactoryController::repairMachine()");
        }

        // ── 선택 장비 요약 ──
        ImGui::Separator();
        const MachineStatusViewData& machine = viewportData.machines[request.selectedMachineIndex];
        ImGui::Text("Machine: %s", machine.name.c_str());
        ImGui::Text("State:   %s", StateLabel(machine.state));
        ImGui::Text("Input:   %s", machine.inputProduct.c_str());
        ImGui::Text("Output:  %s", machine.outputProduct.c_str());
        ImGui::Text("Queue:   %d", machine.waitingCount);
        ImGui::Text("Power:   %s", machine.powerOn ? "ON" : "OFF");

        // ── 요청 로그 ──
        ImGui::Separator();
        ImGui::Text("Request Log");
        for (const std::string& msg : log_) {
            ImGui::BulletText("%s", msg.c_str());
        }

        ImGui::End();
    }

private:
    std::vector<std::string> log_ = {"UI connected to FactoryController."};

    void AddLog(const std::string& message) {
        log_.push_back(message);
        if (log_.size() > 8) log_.erase(log_.begin());
    }
};

class MachineStatusWindow {
public:
    void Render(const std::array<MachineStatusViewData, 4>& machines, int& selectedIndex) {
        ImGui::SetNextWindowPos(ImVec2(930, 486), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(322, 310), ImGuiCond_FirstUseEver);
        ImGui::Begin("Machine Status");

        for (int i = 0; i < static_cast<int>(machines.size()); ++i) {
            const MachineStatusViewData& machine = machines[i];
            bool selected = (i == selectedIndex);

            ImGui::PushID(i);
            if (selected) {
                ImGui::PushStyleColor(ImGuiCol_Button, StateColor(machine.state));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, StateColor(machine.state));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, StateColor(machine.state));
            }

            float bw = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;
            if (ImGui::Button(machine.name.c_str(), ImVec2(bw, 30.0f))) {
                selectedIndex = i;
            }

            if (selected) ImGui::PopStyleColor(3);
            if (i % 2 == 0) ImGui::SameLine();
            ImGui::PopID();
        }

        const MachineStatusViewData& sel = machines[selectedIndex];
        ImGui::Separator();
        ImGui::TextColored(StateColor(sel.state), "%s", sel.name.c_str());
        ImGui::Text("State: %s", StateLabel(sel.state));
        ImGui::ProgressBar(sel.progress, ImVec2(-1.0f, 22.0f), "Progress");
        ImGui::Text("Durability: %.0f%%", sel.durability);
        ImGui::ProgressBar(sel.durability / 100.0f, ImVec2(-1.0f, 18.0f));
        ImGui::Text("Defect rate: %.1f%%", sel.defectRate);
        ImGui::Text("Process time: %.0f tick", sel.processTime);
        ImGui::Text("Remaining: %.0f tick", sel.remainingTime);
        ImGui::Text("Queue: %d", sel.waitingCount);
        ImGui::Text("Power: %s", sel.powerOn ? "ON" : "OFF");

        ImGui::End();
    }
};

class ProductStatusWindow {
public:
    void Render(const std::array<ProductStatusViewData, 5>& products) {
        ImGui::SetNextWindowPos(ImVec2(382, 486), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(530, 290), ImGuiCond_FirstUseEver);
        ImGui::Begin("Product Status");

        for (const ProductStatusViewData& product : products) {
            ImGui::PushStyleColor(ImGuiCol_Text, product.color);
            ImGui::BulletText("%s", product.name.c_str());
            ImGui::PopStyleColor();
            ImGui::SameLine(170.0f);
            ImGui::Text("Count: %d", product.count);
            if (product.kind == ProductKind::CPU) {
                ImGui::SameLine(285.0f);
                ImGui::Text("Defect: %d", product.defectiveCount);
            }
        }

        ImGui::End();
    }
};

class FactoryViewportWindow {
public:
    void Render(const ViewportViewData& data) {
        ImGui::SetNextWindowPos(ImVec2(382, 16), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(870, 455), ImGuiCond_FirstUseEver);
        ImGui::Begin("Viewport");

        ImVec2 origin = ImGui::GetCursorScreenPos();
        ImVec2 canvasSize(std::max(780.0f, ImGui::GetContentRegionAvail().x), 390.0f);
        ImDrawList* draw = ImGui::GetWindowDrawList();

        draw->AddRectFilled(origin, ImVec2(origin.x + canvasSize.x, origin.y + canvasSize.y), IM_COL32(24, 29, 36, 255), 8.0f);
        draw->AddRect(origin, ImVec2(origin.x + canvasSize.x, origin.y + canvasSize.y), IM_COL32(72, 82, 96, 255), 8.0f);

        const float stationW = 160.0f;
        const float stationH = 112.0f;
        const float left = origin.x + 46.0f;
        const float top  = origin.y + 128.0f;
        const float gap  = (canvasSize.x - 92.0f - stationW * 4.0f) / 3.0f;

        std::array<ImVec2, 4> centers;
        for (int i = 0; i < static_cast<int>(data.machines.size()); ++i) {
            const MachineStatusViewData& machine = data.machines[i];
            ImVec2 min(left + i * (stationW + gap), top);
            ImVec2 max(min.x + stationW, min.y + stationH);
            centers[i] = ImVec2((min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f);

            bool active = (i == data.activeMachineIndex);
            ImVec4 stateColor = StateColor(machine.state);
            ImU32 glow = ToColor(stateColor);

            draw->AddRectFilled(ImVec2(min.x - 8.0f, min.y - 8.0f), ImVec2(max.x + 8.0f, max.y + 8.0f),
                                DimColor(stateColor, active ? 0.55f : 0.30f), 10.0f);
            draw->AddRectFilled(min, max, active ? IM_COL32(67, 84, 104, 255) : IM_COL32(43, 52, 64, 255), 7.0f);
            draw->AddRect(min, max, active ? glow : IM_COL32(132, 145, 160, 160), 7.0f, 0, active ? 3.0f : 1.0f);
            draw->AddText(ImVec2(min.x + 12.0f, min.y + 14.0f), IM_COL32(250, 252, 255, 255), machine.name.c_str());
            draw->AddText(ImVec2(min.x + 12.0f, min.y + 38.0f), IM_COL32(230, 236, 243, 220), StateLabel(machine.state));

            char info[64];
            std::snprintf(info, sizeof(info), "D %.0f%%  F %.1f%%", machine.durability, machine.defectRate);
            draw->AddText(ImVec2(min.x + 12.0f, min.y + 60.0f), IM_COL32(230, 236, 243, 220), info);

            char queueInfo[64];
            std::snprintf(queueInfo, sizeof(queueInfo), "Q %d  T %.0f", machine.waitingCount, machine.remainingTime);
            draw->AddText(ImVec2(min.x + 12.0f, min.y + 82.0f), IM_COL32(230, 236, 243, 220), queueInfo);

            ImVec2 barMin(min.x + 12.0f, max.y + 14.0f);
            ImVec2 barMax(max.x - 12.0f, max.y + 28.0f);
            draw->AddRectFilled(barMin, barMax, IM_COL32(54, 60, 68, 255), 4.0f);
            draw->AddRectFilled(barMin, ImVec2(barMin.x + (barMax.x - barMin.x) * machine.progress, barMax.y), glow, 4.0f);
            draw->AddRect(barMin, barMax, IM_COL32(190, 200, 212, 120), 4.0f);
        }

        for (int i = 0; i < 3; ++i) {
            draw->AddLine(centers[i], centers[i + 1], IM_COL32(121, 137, 158, 180), 5.0f);
            draw->AddCircleFilled(centers[i + 1], 5.0f, IM_COL32(121, 137, 158, 230));
        }

        const ProductStatusViewData& product = data.products[data.activeProductIndex];
        ImVec2 marker = centers[data.activeMachineIndex];
        if (product.kind == ProductKind::CPU) {
            DrawChip(draw, ImVec2(marker.x, marker.y - 76.0f), 58.0f);
        } else {
            DrawWafer(draw, ImVec2(marker.x, marker.y - 76.0f), 30.0f, ToColor(product.color));
        }

        ImGui::Dummy(canvasSize);
        ImGui::End();
    }
};

class EventLogWindow {
public:
    void Render(const std::vector<LogEntry>& entries, bool& clearRequested) {
        ImGui::SetNextWindowPos(ImVec2(16, 566), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(350, 206), ImGuiCond_FirstUseEver);
        ImGui::Begin("Event Log");

        if (ImGui::Button("Clear Log", ImVec2(-1.0f, 26.0f))) {
            clearRequested = true;
            selected_ = -1;
        }
        ImGui::Separator();

        ImGui::BeginChild("log_scroll", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
        bool atBottom = ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 4.0f;
        for (int i = 0; i < static_cast<int>(entries.size()); ++i) {
            const LogEntry& entry = entries[i];
            ImGui::PushStyleColor(ImGuiCol_Text, LogColor(entry.level));
            if (ImGui::Selectable(entry.toString().c_str(), selected_ == i)) {
                selected_ = i;
            }
            ImGui::PopStyleColor();
        }
        if (atBottom && static_cast<int>(entries.size()) != lastCount_) {
            ImGui::SetScrollHereY(1.0f);
        }
        lastCount_ = static_cast<int>(entries.size());
        ImGui::EndChild();

        ImGui::End();
    }

private:
    int selected_  = -1;
    int lastCount_ = 0;

    static ImVec4 LogColor(LogLevel level) {
        if (level == LOG_ERROR)   return ImVec4(0.92f, 0.35f, 0.35f, 1.0f);
        if (level == LOG_WARNING) return ImVec4(0.95f, 0.78f, 0.30f, 1.0f);
        return ImVec4(0.78f, 0.84f, 0.90f, 1.0f);
    }
};

class StatisticsWindow {
public:
    void Render(const ViewportViewData& data) {
        ImGui::SetNextWindowPos(ImVec2(16, 436), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(350, 120), ImGuiCond_FirstUseEver);
        ImGui::Begin("Statistics");
        ImGui::Text("Finished goods  : %d", data.finishedGoods);
        ImGui::Text("WIP (in system) : %d", data.wipCount);
        ImGui::Text("Total breakdowns: %d", data.totalBreakdowns);
        ImGui::Text("Lost products   : %d", data.lostProducts);
        ImGui::End();
    }
};

// ─────────────────────────────────────────────────────────────
// 최상위 앱 클래스: FactoryController + 모든 UI 창 통합
// ─────────────────────────────────────────────────────────────

class CpuFactoryApp {
public:
    void Render() {
        static float accumulator = 0.0f;
        accumulator += ImGui::GetIO().DeltaTime;
        // 속도 슬라이더(1x~5x)에 따라 틱 간격을 조절한다.
        const float tickInterval = 0.1f / static_cast<float>(std::max(1, request_.simulationSpeed));
        while (accumulator >= tickInterval) {
            controller_.update();
            accumulator -= tickInterval;
        }

        ViewportViewData vd = BuildViewData(controller_, request_.selectedMachineIndex);
        for (int i = 0; i < 4; ++i) {
            vd.machines[i].powerOn = request_.machinePowerOn[i];
        }

        // ③ UI 요청 처리 (carrier-pigeon 명령을 백엔드에 전달)
        if (request_.requestStart)       { controller_.start();                                       request_.requestStart       = false; }
        if (request_.requestPause)       { controller_.pause();                                       request_.requestPause       = false; }
        if (request_.requestReset)       { controller_.reset();                                       request_.requestReset       = false; }
        if (request_.requestAddRawWafer) { controller_.addRawWafer(request_.rawWaferAmount);          request_.requestAddRawWafer = false; }
        if (request_.requestRepair)      { controller_.repairMachine(request_.selectedMachineIndex);  request_.requestRepair      = false; }
        if (request_.requestForceBreak)  { controller_.forceBreakMachine(request_.selectedMachineIndex); request_.requestForceBreak = false; }
        if (request_.requestClearLog)    { controller_.clearLog();                                    request_.requestClearLog    = false; }
        if (request_.requestScenarioChange) {
            controller_.updateCase(static_cast<Case>(request_.selectedScenario));
            request_.requestScenarioChange = false;
        }
        if (request_.requestPowerToggle) {
            controller_.setPower(request_.selectedMachineIndex,
                                        request_.machinePowerOn[request_.selectedMachineIndex]);
            request_.requestPowerToggle = false;
        }

        controlPanel_.Render(request_, vd, controller_.isRunning());
        viewport_.Render(vd);
        productStatus_.Render(vd.products);
        machineStatus_.Render(vd.machines, request_.selectedMachineIndex);
        statistics_.Render(vd);
        eventLog_.Render(controller_.getLogEntries(), request_.requestClearLog);
    }

    bool IsDarkMode() const { return true; }

private:
    Bridge               controller_;
    ControlPanelRequest  request_;
    ControlPanelWindow   controlPanel_;
    MachineStatusWindow  machineStatus_;
    ProductStatusWindow  productStatus_;
    FactoryViewportWindow viewport_;
    StatisticsWindow     statistics_;
    EventLogWindow       eventLog_;
};

// ─────────────────────────────────────────────────────────────
// main
// ─────────────────────────────────────────────────────────────

int main(int argc, char** argv) {
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_Window* window = SDL_CreateWindow(
        "CPU Factory Simulator",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280,
        780,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    if (!window) {
        SDL_Quit();
        return -1;
    }

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 8.0f;
    style.FrameRounding  = 5.0f;
    style.GrabRounding   = 5.0f;
    style.WindowPadding  = ImVec2(14.0f, 12.0f);

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 120");

    CpuFactoryApp app;
    bool done = false;

    while (!done) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) {
                done = true;
            }
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(window)) {
                done = true;
            }
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        app.Render();

        ImGui::Render();
        glClearColor(0.10f, 0.11f, 0.13f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}