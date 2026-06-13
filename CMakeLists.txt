cmake_minimum_required(VERSION 3.10)

project(IMGuiApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/imgui")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(MODELS_DIR "${SRC_DIR}/Models")

set(SOURCES
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/Bridge.cpp
    ${SRC_DIR}/Controller.cpp
    ${SRC_DIR}/Initializer.cpp
    ${SRC_DIR}/Logger.cpp

    ${MODELS_DIR}/Root.cpp
    ${MODELS_DIR}/Generator.cpp
    ${MODELS_DIR}/Machines/Machine.cpp
    ${MODELS_DIR}/Machines/Durability.cpp
    ${MODELS_DIR}/Machines/Photolithography.cpp
    ${MODELS_DIR}/Machines/Etcher.cpp
    ${MODELS_DIR}/Machines/IonImplantator.cpp
    ${MODELS_DIR}/Machines/Packager.cpp
    ${MODELS_DIR}/Products/Product.cpp
    ${MODELS_DIR}/Products/RawWafer.cpp
    ${MODELS_DIR}/Products/PatternedWafer.cpp
    ${MODELS_DIR}/Products/EtchedWafer.cpp
    ${MODELS_DIR}/Products/DopedWafer.cpp
    ${MODELS_DIR}/Products/CPU.cpp
    ${SRC_DIR}/ProgressQueue.cpp

    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp

    ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

add_executable(IMGuiApp ${SOURCES})

target_include_directories(IMGuiApp PRIVATE
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${SRC_DIR}
)

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

target_include_directories(IMGuiApp PRIVATE
    ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(IMGuiApp PRIVATE
    ${SDL2_LIBRARIES}
    OpenGL::GL
)

if(APPLE)
    target_link_libraries(IMGuiApp PRIVATE
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
endif()
