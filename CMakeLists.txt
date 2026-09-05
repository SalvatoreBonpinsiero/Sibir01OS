cmake_minimum_required(VERSION 3.15)
project(ESP32Dashboard)

set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends
)

set(IMGUI_SOURCES
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)

add_executable(ESP32Dashboard main.cpp ${IMGUI_SOURCES})

target_link_libraries(ESP32Dashboard PRIVATE glfw opengl32 gdi32 user32 shell32 -static)
