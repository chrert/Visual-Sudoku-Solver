cmake_minimum_required(VERSION 2.8)

project(vsudoku)

find_file(TRAINING_SET_PATH training_set ${CMAKE_CURRENT_SOURCE_DIR})

add_definitions(-std=c++11
                -DTRAINING_DATA_DIR="${TRAINING_SET_PATH}/")

set(CMAKE_BUILD_TYPE "DEBUG")

file(GLOB_RECURSE SOURCES *.cpp)
file(GLOB_RECURSE HEADERS *.hpp)

include_directories(include)

find_package(OpenCV REQUIRED)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Declarative REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB_RECURSE QT_FORMS *.ui)

qt5_wrap_ui(QT_FORMS_HEADERS ${QT_FORMS})

add_executable(vsudoku
               ${SOURCES}
               ${HEADERS}
               ${QT_FORMS_HEADERS})

qt5_use_modules(vsudoku Widgets Declarative)

target_link_libraries(vsudoku ${OpenCV_LIBS})

