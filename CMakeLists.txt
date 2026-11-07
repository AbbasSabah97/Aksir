cmake_minimum_required(VERSION 3.16)

project(Aksir VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(Aksir
    main.cpp
    spreadsheetmodel.cpp
    spreadsheetmodel.h
    spreadsheetwindow.cpp
    spreadsheetwindow.h
)

target_link_libraries(Aksir PRIVATE Qt6::Widgets)

if(WIN32)
    set_target_properties(Aksir PROPERTIES WIN32_EXECUTABLE TRUE)
endif()
