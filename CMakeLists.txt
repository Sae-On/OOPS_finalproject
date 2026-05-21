cmake_minimum_required(VERSION 3.10)

project(IMGuiApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/imgui")

set(SOURCES
    main.cpp

    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp

    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

add_executable(IMGuiApp ${SOURCES})

target_include_directories(IMGuiApp PRIVATE
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(GLFW REQUIRED glfw3)

target_include_directories(IMGuiApp PRIVATE
    ${GLFW_INCLUDE_DIRS}
)

target_link_directories(IMGuiApp PRIVATE
    ${GLFW_LIBRARY_DIRS}
)

target_link_libraries(IMGuiApp PRIVATE
    ${GLFW_LIBRARIES}
    OpenGL::GL
)

if(APPLE)
    target_link_libraries(IMGuiApp PRIVATE
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
endif()