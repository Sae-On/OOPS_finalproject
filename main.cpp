#define GL_SILENCE_DEPRECATION

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>

struct ProcessStep {
    const char* name;
    const char* description;
    float duration;
    float defectRisk;
    ImVec4 color;
};

struct Batch {
    bool active = false;
    int step = 0;
    float progress = 0.0f;
    float yield = 1.0f;
    int wafers = 0;
};

static ImU32 ColorU32(const ImVec4& color) {
    return ImGui::ColorConvertFloat4ToU32(color);
}

static ImU32 BlendColor(const ImVec4& color, float factor) {
    return ImGui::ColorConvertFloat4ToU32(ImVec4(
        std::clamp(color.x * factor, 0.0f, 1.0f),
        std::clamp(color.y * factor, 0.0f, 1.0f),
        std::clamp(color.z * factor, 0.0f, 1.0f),
        color.w
    ));
}

static void DrawChip(ImDrawList* draw, ImVec2 center, float size, ImU32 body, ImU32 pin) {
    const float half = size * 0.5f;
    ImVec2 min(center.x - half, center.y - half);
    ImVec2 max(center.x + half, center.y + half);

    for (int i = 0; i < 6; ++i) {
        float t = min.y + 8.0f + i * (size - 16.0f) / 5.0f;
        draw->AddLine(ImVec2(min.x - 8.0f, t), ImVec2(min.x, t), pin, 2.0f);
        draw->AddLine(ImVec2(max.x, t), ImVec2(max.x + 8.0f, t), pin, 2.0f);
    }
    for (int i = 0; i < 6; ++i) {
        float t = min.x + 8.0f + i * (size - 16.0f) / 5.0f;
        draw->AddLine(ImVec2(t, min.y - 8.0f), ImVec2(t, min.y), pin, 2.0f);
        draw->AddLine(ImVec2(t, max.y), ImVec2(t, max.y + 8.0f), pin, 2.0f);
    }

    draw->AddRectFilled(min, max, body, 6.0f);
    draw->AddRect(min, max, IM_COL32(255, 255, 255, 150), 6.0f, 0, 2.0f);
    draw->AddRectFilled(ImVec2(min.x + 14.0f, min.y + 14.0f), ImVec2(max.x - 14.0f, max.y - 14.0f), IM_COL32(30, 38, 48, 255), 4.0f);
    draw->AddText(ImVec2(center.x - 16.0f, center.y - 7.0f), IM_COL32(240, 248, 255, 255), "CPU");
}

static void DrawWafer(ImDrawList* draw, ImVec2 center, float radius, float yield) {
    draw->AddCircleFilled(center, radius, IM_COL32(196, 215, 228, 255), 48);
    draw->AddCircle(center, radius, IM_COL32(255, 255, 255, 180), 48, 2.0f);
    draw->AddCircle(center, radius * 0.32f, IM_COL32(112, 140, 158, 120), 32, 1.0f);

    const int grid = 7;
    const float cell = radius * 0.28f;
    ImU32 good = IM_COL32(71, 196, 148, 210);
    ImU32 weak = IM_COL32(238, 107, 94, 210);
    for (int y = -3; y <= 3; ++y) {
        for (int x = -3; x <= 3; ++x) {
            ImVec2 p(center.x + x * cell, center.y + y * cell);
            float dist = std::sqrt((p.x - center.x) * (p.x - center.x) + (p.y - center.y) * (p.y - center.y));
            if (dist > radius * 0.78f) {
                continue;
            }
            bool isGood = ((x + 3) + (y + 3) * grid) / 49.0f < yield;
            draw->AddRectFilled(ImVec2(p.x - cell * 0.36f, p.y - cell * 0.36f),
                                ImVec2(p.x + cell * 0.36f, p.y + cell * 0.36f),
                                isGood ? good : weak, 2.0f);
        }
    }
}

static void DrawFactoryMap(const std::array<ProcessStep, 9>& steps, const Batch& batch) {
    ImGui::Begin("Factory Floor");

    ImVec2 origin = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize(std::max(780.0f, ImGui::GetContentRegionAvail().x), 390.0f);
    ImDrawList* draw = ImGui::GetWindowDrawList();

    draw->AddRectFilled(origin, ImVec2(origin.x + canvasSize.x, origin.y + canvasSize.y), IM_COL32(24, 29, 36, 255), 8.0f);
    draw->AddRect(origin, ImVec2(origin.x + canvasSize.x, origin.y + canvasSize.y), IM_COL32(72, 82, 96, 255), 8.0f);

    const float left = origin.x + 48.0f;
    const float top = origin.y + 68.0f;
    const float stationW = 150.0f;
    const float stationH = 78.0f;
    const float gap = 28.0f;
    const int columns = 5;

    std::array<ImVec2, 9> centers;
    for (int i = 0; i < static_cast<int>(steps.size()); ++i) {
        int row = i / columns;
        int col = i % columns;
        if (row == 1) {
            col = columns - 1 - col;
        }

        ImVec2 min(left + col * (stationW + gap), top + row * 158.0f);
        ImVec2 max(min.x + stationW, min.y + stationH);
        centers[i] = ImVec2((min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f);

        bool active = batch.active && batch.step == i;
        ImU32 fill = active ? ColorU32(steps[i].color) : BlendColor(steps[i].color, 0.38f);
        draw->AddRectFilled(min, max, fill, 7.0f);
        draw->AddRect(min, max, active ? IM_COL32(255, 255, 255, 255) : IM_COL32(132, 145, 160, 160), 7.0f, 0, active ? 3.0f : 1.0f);
        draw->AddText(ImVec2(min.x + 12.0f, min.y + 13.0f), IM_COL32(250, 252, 255, 255), steps[i].name);

        char buffer[32];
        std::snprintf(buffer, sizeof(buffer), "Risk %.1f%%", steps[i].defectRisk * 100.0f);
        draw->AddText(ImVec2(min.x + 12.0f, min.y + 44.0f), IM_COL32(230, 236, 243, 220), buffer);
    }

    for (int i = 0; i < 8; ++i) {
        draw->AddLine(centers[i], centers[i + 1], IM_COL32(121, 137, 158, 180), 5.0f);
        draw->AddCircleFilled(centers[i + 1], 5.0f, IM_COL32(121, 137, 158, 230));
    }

    if (batch.active) {
        ImVec2 p = centers[batch.step];
        if (batch.step < 8) {
            ImVec2 next = centers[batch.step + 1];
            p.x = p.x + (next.x - p.x) * batch.progress;
            p.y = p.y + (next.y - p.y) * batch.progress;
        }
        DrawWafer(draw, ImVec2(p.x, p.y - 62.0f), 28.0f, batch.yield);
    }

    DrawChip(draw, ImVec2(origin.x + canvasSize.x - 82.0f, origin.y + canvasSize.y - 64.0f), 58.0f, IM_COL32(46, 98, 120, 255), IM_COL32(218, 224, 229, 255));
    draw->AddText(ImVec2(origin.x + 30.0f, origin.y + 24.0f), IM_COL32(235, 242, 250, 255), "CPU manufacturing line: wafer -> patterning -> wiring -> test -> package");

    ImGui::Dummy(canvasSize);
    ImGui::End();
}

int main() {
    if (!glfwInit()) {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    GLFWwindow* window = glfwCreateWindow(1280, 780, "CPU Factory Simulator", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 8.0f;
    style.FrameRounding = 5.0f;
    style.GrabRounding = 5.0f;
    style.WindowPadding = ImVec2(14.0f, 12.0f);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 120");

    const std::array<ProcessStep, 9> steps = {{
        {"Wafer Prep", "Grow silicon ingot, slice it into wafers, then polish the surface.", 2.0f, 0.010f, ImVec4(0.32f, 0.55f, 0.78f, 1.0f)},
        {"Oxide/Film", "Build thin insulating or conductive layers on the wafer.", 2.5f, 0.014f, ImVec4(0.55f, 0.48f, 0.80f, 1.0f)},
        {"Lithography", "Coat photoresist and expose circuit patterns through a mask.", 3.3f, 0.026f, ImVec4(0.83f, 0.50f, 0.42f, 1.0f)},
        {"Etch/Implant", "Remove selected material and dope silicon to form transistor regions.", 2.8f, 0.024f, ImVec4(0.79f, 0.62f, 0.30f, 1.0f)},
        {"Interconnect", "Deposit and pattern metal wiring layers that connect transistors.", 3.4f, 0.030f, ImVec4(0.42f, 0.70f, 0.57f, 1.0f)},
        {"Wafer Probe", "Electrically test each die while it is still on the wafer.", 1.8f, 0.012f, ImVec4(0.38f, 0.65f, 0.76f, 1.0f)},
        {"Dicing", "Cut the wafer into individual CPU dies.", 1.4f, 0.018f, ImVec4(0.69f, 0.56f, 0.43f, 1.0f)},
        {"Packaging", "Attach die, bond connections, add heat spreader, and seal package.", 2.6f, 0.022f, ImVec4(0.37f, 0.59f, 0.46f, 1.0f)},
        {"Final Test", "Run performance tests, bin chips, and ship only passing CPUs.", 2.2f, 0.016f, ImVec4(0.45f, 0.58f, 0.84f, 1.0f)},
    }};

    bool running = false;
    bool darkMode = true;
    int wafers = 8;
    int wafersPerBatch = 1;
    int diesPerWafer = 64;
    int finishedCpus = 0;
    int defects = 0;
    int completedBatches = 0;
    float speed = 1.0f;
    float processQuality = 92.0f;
    Batch batch;
    std::vector<std::string> log = {"System ready. Add wafers, start the line, and watch yield change."};

    auto pushLog = [&log](const std::string& message) {
        log.push_back(message);
        if (log.size() > 9) {
            log.erase(log.begin());
        }
    };

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        float dt = ImGui::GetIO().DeltaTime;
        if (running && !batch.active && wafers >= wafersPerBatch) {
            wafers -= wafersPerBatch;
            batch = Batch{};
            batch.active = true;
            batch.wafers = wafersPerBatch;
            pushLog("New wafer batch entered Wafer Prep.");
        }

        if (running && batch.active) {
            const ProcessStep& step = steps[batch.step];
            batch.progress += (dt * speed) / step.duration;
            if (batch.progress >= 1.0f) {
                float qualityBonus = std::clamp(processQuality / 100.0f, 0.55f, 1.0f);
                float loss = step.defectRisk * (1.25f - qualityBonus);
                batch.yield = std::clamp(batch.yield * (1.0f - loss), 0.0f, 1.0f);

                char message[128];
                std::snprintf(message, sizeof(message), "%s completed. Estimated yield: %.1f%%", step.name, batch.yield * 100.0f);
                pushLog(message);

                batch.step++;
                batch.progress = 0.0f;

                if (batch.step >= static_cast<int>(steps.size())) {
                    int totalDies = batch.wafers * diesPerWafer;
                    int good = static_cast<int>(std::round(totalDies * batch.yield));
                    finishedCpus += good;
                    defects += totalDies - good;
                    completedBatches++;
                    pushLog("Batch finished: CPUs passed final test and defects were counted.");
                    batch = Batch{};
                }
            }
        }

        ImGui::SetNextWindowPos(ImVec2(16, 16), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(350, 520), ImGuiCond_FirstUseEver);
        ImGui::Begin("Control Panel");
        ImGui::Text("CPU Factory Simulator");
        ImGui::TextColored(ImVec4(0.70f, 0.82f, 0.95f, 1.0f), "OOPS front-end prototype");
        ImGui::Separator();

        if (ImGui::Button(running ? "Pause Line" : "Start Line", ImVec2(150, 32))) {
            running = !running;
        }
        ImGui::SameLine();
        if (ImGui::Button("Add 5 Wafers", ImVec2(150, 32))) {
            wafers += 5;
            pushLog("Inventory received 5 new wafers.");
        }

        if (ImGui::Button("Reset Simulation", ImVec2(308, 30))) {
            running = false;
            wafers = 8;
            finishedCpus = 0;
            defects = 0;
            completedBatches = 0;
            batch = Batch{};
            log.clear();
            pushLog("Simulation reset.");
        }

        ImGui::Separator();
        ImGui::SliderFloat("Line speed", &speed, 0.25f, 4.0f, "%.2fx");
        ImGui::SliderFloat("Process quality", &processQuality, 70.0f, 99.5f, "%.1f%%");
        ImGui::SliderInt("Dies per wafer", &diesPerWafer, 24, 128);
        ImGui::SliderInt("Wafers per batch", &wafersPerBatch, 1, 4);
        wafersPerBatch = std::max(1, wafersPerBatch);

        ImGui::Separator();
        ImGui::Text("Line: %s", running ? "RUNNING" : "STOPPED");
        ImGui::Text("Wafer inventory: %d", wafers);
        ImGui::Text("Finished CPUs: %d", finishedCpus);
        ImGui::Text("Defective dies: %d", defects);
        ImGui::Text("Completed batches: %d", completedBatches);

        float total = static_cast<float>(finishedCpus + defects);
        float actualYield = total > 0.0f ? (finishedCpus / total) : 0.0f;
        ImGui::ProgressBar(actualYield, ImVec2(-1.0f, 22.0f), "Actual yield");
        ImGui::Text("%.1f%% pass rate", actualYield * 100.0f);

        ImGui::Checkbox("Dark background", &darkMode);
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(382, 16), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(870, 455), ImGuiCond_FirstUseEver);
        DrawFactoryMap(steps, batch);

        ImGui::SetNextWindowPos(ImVec2(382, 486), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(530, 260), ImGuiCond_FirstUseEver);
        ImGui::Begin("Manufacturing Process");
        ImGui::TextWrapped("A CPU is not built in one step. The simulator treats it as a batch flow: prepare a silicon wafer, repeatedly form transistor patterns and wiring, test each die, cut the wafer, package the chips, then run final quality tests.");
        ImGui::Separator();
        for (int i = 0; i < static_cast<int>(steps.size()); ++i) {
            bool active = batch.active && batch.step == i;
            ImGui::PushStyleColor(ImGuiCol_Text, active ? ImVec4(1.0f, 0.96f, 0.72f, 1.0f) : ImVec4(0.86f, 0.90f, 0.95f, 1.0f));
            ImGui::BulletText("%02d. %s", i + 1, steps[i].name);
            ImGui::PopStyleColor();
            ImGui::SameLine(150.0f);
            ImGui::TextWrapped("%s", steps[i].description);
        }
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(930, 486), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(322, 260), ImGuiCond_FirstUseEver);
        ImGui::Begin("Batch Monitor");
        if (batch.active) {
            const ProcessStep& step = steps[batch.step];
            ImGui::Text("Current step: %s", step.name);
            ImGui::ProgressBar(batch.progress, ImVec2(-1.0f, 24.0f));
            ImGui::Text("Batch wafers: %d", batch.wafers);
            ImGui::Text("Estimated yield: %.1f%%", batch.yield * 100.0f);
            ImGui::Text("Expected CPUs: %.0f", std::round(batch.wafers * diesPerWafer * batch.yield));
        } else {
            ImGui::TextWrapped("No active batch. Start the line when wafer inventory is available.");
        }
        ImGui::Separator();
        ImGui::Text("Factory Log");
        for (const std::string& entry : log) {
            ImGui::BulletText("%s", entry.c_str());
        }
        ImGui::End();

        ImGui::Render();

        if (darkMode) {
            glClearColor(0.10f, 0.11f, 0.13f, 1.0f);
        } else {
            glClearColor(0.80f, 0.83f, 0.86f, 1.0f);
        }

        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
