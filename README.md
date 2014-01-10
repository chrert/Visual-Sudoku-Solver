Visual Sudoku Solver
============

This project aims to develop an application that finds sudoku fields in videos taken from a webcam and solves them.
It uses OpenCV for the image processing and Machine Learning parts and Qt as underlying architecture.

Software Requirements
------------

* Qt4 or Qt5
* OpenCV 2.4.8 (maybe also other versions)


Compiling
------------

This project uses CMake as build environment. See the [CMake Documentation](http://www.cmake.org/cmake/help/documentation.html) for details.
The following steps are necessary to build the application:

```bash
mkdir ../build
cd ../build
cmake ../vsudoku -DUSE_QT_5=1
make
```

This will build the application using Qt5. If you prefer to use Qt4 just omit ``-DUSEQT_QT_5=1``
