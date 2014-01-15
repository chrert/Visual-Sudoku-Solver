cmake_minimum_required(VERSION 2.8)

project(vsudoku)

find_file(TRAINING_SET_PATH training_set ${CMAKE_CURRENT_SOURCE_DIR})

add_definitions(-std=c++11
                -pedantic
                -O3
                -DTRAINING_DATA_DIR="${TRAINING_SET_PATH}/"
                -DUSE_SVM_CLASSIFIER)

set(CMAKE_BUILD_TYPE "RELEASE")

macro(add_sources)
    file(RELATIVE_PATH _relPath "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
    foreach(_src ${ARGN})
        if(_relPath)
            list(APPEND SOURCES "${_relPath}/${_src}")
        else()
            list(APPEND SOURCES "${_src}")
        endif()
    endforeach()
    if(_relPath)
        # propagate SRCS to parent directory
        set(SOURCES ${SOURCES} PARENT_SCOPE)
    endif()
endmacro()

macro(add_headers)
    file(RELATIVE_PATH _relPath "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
    foreach(_src ${ARGN})
        if(_relPath)
            list(APPEND HEADERS "${_relPath}/${_src}")
        else()
            list(APPEND HEADERS "${_src}")
        endif()
    endforeach()
    if(_relPath)
        # propagate SRCS to parent directory
        set(HEADERS ${HEADERS} PARENT_SCOPE)
    endif()
endmacro()

macro(add_qtforms)
    file(RELATIVE_PATH _relPath "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
    foreach(_src ${ARGN})
        if(_relPath)
            list(APPEND QT_FORMS "${_relPath}/${_src}")
        else()
            list(APPEND QT_FORMS "${_src}")
        endif()
    endforeach()
    if(_relPath)
        # propagate SRCS to parent directory
        set(QT_FORMS ${QT_FORMS} PARENT_SCOPE)
    endif()
endmacro()

macro(forward_vars)
    set(SOURCES ${SOURCES} PARENT_SCOPE)
    set(HEADERS ${HEADERS} PARENT_SCOPE)
    set(QT_FORMS ${QT_FORMS} PARENT_SCOPE)
endmacro()

add_subdirectory(source)
add_subdirectory(include)
add_subdirectory(ui)

include_directories(include)

find_package(OpenCV REQUIRED)
set(LIBS ${OpenCV_LIBS})

if(${USE_QT_5} MATCHES "1")
  message("Using Qt5")
  find_package(Qt5Widgets REQUIRED)
  qt5_wrap_ui(QT_FORMS_HEADERS ${QT_FORMS})
else()
  message("Using Qt4")
  find_package(Qt4 REQUIRED)
  include(${QT_USE_FILE})
  qt4_wrap_ui(QT_FORMS_HEADERS ${QT_FORMS})
  set(LIBS ${LIBS} ${QT_LIBRARIES})
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(vsudoku
               ${SOURCES}
               ${HEADERS}
               ${QT_FORMS_HEADERS})

if(${USE_QT_5} MATCHES "1")
  qt5_use_modules(vsudoku Widgets)
endif()

target_link_libraries(vsudoku ${LIBS})

