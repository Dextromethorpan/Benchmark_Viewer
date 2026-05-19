cmake_minimum_required(VERSION 3.20)
project(BenchmarkViewer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
qt_standard_project_setup()

qt_add_executable(BenchmarkViewer
    main.cpp
    MainWindow.hpp
)

target_link_libraries(BenchmarkViewer
    PRIVATE
        Qt6::Core
        Qt6::Widgets
)

# Windows: hide the console window
if(WIN32)
    set_target_properties(BenchmarkViewer PROPERTIES
        WIN32_EXECUTABLE TRUE)
endif()