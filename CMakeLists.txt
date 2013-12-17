cmake_minimum_required(VERSION 2.8)

project(vsudoku)

add_definitions(-std=c++11)

set(CMAKE_BUILD_TYPE "RELEASE")


set(SOURCES main.cpp
            sudokufinder.cpp
            mainwindow.cpp)
set(HEADERS sudokufinder.hpp
            geometricutils.hpp
            drawutils.hpp)

set(QT_SOURCES mainwindow.cpp)
set(QT_HEADERS mainwindow.hpp
               qtopencv.hpp)

find_package(OpenCV REQUIRED)

find_package(Qt4 REQUIRED)
qt4_wrap_cpp(HEADERS_MOC ${QT_HEADERS})
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})
set(QT_FORMS mainwindow.ui)
set(QT_RESOURCES)
qt4_wrap_ui(QT_FORMS_HEADERS ${QT_FORMS})
qt4_add_resources(QT_RESOURCES_RCC ${QT_RESOURCES})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(vsudoku
               ${SOURCES}
               ${QT_SOURCES}
               ${HEADERS}
               ${HEADERS_MOC}
               ${QT_FORMS_HEADERS}
               ${QT_RESOURCES_RCC})


target_link_libraries(vsudoku ${OpenCV_LIBS} ${QT_LIBRARIES})

