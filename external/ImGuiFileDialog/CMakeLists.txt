cmake_minimum_required (VERSION 3.8)

add_library(ImGuiFileDialog STATIC ImGuiFileDialog.cpp)
target_include_directories(ImGuiFileDialog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ImGuiFileDialog PRIVATE imgui::imgui)