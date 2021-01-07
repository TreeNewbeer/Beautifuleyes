cmake_minimum_required(VERSION 3.5)

project(qt_uart_cmake LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_PREFIX_PATH "/home/genius/program/QT/5.15.2/gcc_64/lib/cmake")

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets SerialPort Charts REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        Uart.cpp Uart.h)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(qt_uart_cmake
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(qt_uart_cmake SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(pc
            ${PROJECT_SOURCES}
                SignalChart.cpp SignalChart.h)
    endif()
endif()

target_link_libraries(pc PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::SerialPort  Qt${QT_VERSION_MAJOR}::Charts)
