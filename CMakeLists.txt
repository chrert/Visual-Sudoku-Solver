cmake_minimum_required(VERSION 2.8)

project(vsudoku)

add_definitions(-std=c++11)

set(CMAKE_BUILD_TYPE "DEBUG")

set(SOURCES main.cpp
            sudokufinder.cpp
            digitextractor.cpp
            digitclassifier.cpp
            svmdigitclassifier.cpp
            knndigitclassifier.cpp
            nndigitclassifier.cpp
            mainwindow.cpp
            savedialog.cpp)
set(HEADERS typedefs.hpp
            settings.hpp
            geometricutils.hpp
            drawutils.hpp
            qtopencv.hpp)

find_package(OpenCV REQUIRED)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Declarative REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(QT_FORMS mainwindow.ui
             savedialog.ui)
qt5_wrap_ui(QT_FORMS_HEADERS ${QT_FORMS})

add_executable(vsudoku
               ${SOURCES}
               ${HEADERS}
               ${QT_FORMS_HEADERS})

qt5_use_modules(vsudoku Widgets Declarative)

target_link_libraries(vsudoku ${OpenCV_LIBS})

