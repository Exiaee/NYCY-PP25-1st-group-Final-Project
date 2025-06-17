QT += core gui sql widgets

CONFIG += c++17

INCLUDEPATH += "C:/opencv/build/include"
INCLUDEPATH += "C:/opencv/build/include/opencv2"
INCLUDEPATH += C:/OpenCV411_MinGW_build/include
QMAKE_LFLAGS +=  -fopenmp
QMAKE_CXXFLAGS+=  -fopenmp

LIBS += -LC:/opencv/build/x64/vc16/lib \
     -lopencv_world4110d

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    database.cpp \
    imagephash.cpp \
    newwindow.cpp

HEADERS += \
    mainwindow.h \
    database.h \
    imagephash.h \
    newwindow.h \
    stb_image.h

FORMS += \
    mainwindow.ui \
    newwindow.ui




LIBS += C:/OpenCV411_MinGW_build/x64/mingw/bin/libopencv_core411.dll
LIBS += C:/OpenCV411_MinGW_build/x64/mingw/bin/libopencv_imgcodecs411.dll
LIBS += C:/OpenCV411_MinGW_build/x64/mingw/bin/libopencv_imgproc411.dll
LIBS += C:/OpenCV411_MinGW_build/x64/mingw/bin/libopencv_highgui411.dll
LIBS += C:/OpenCV411_MinGW_build/x64/mingw/bin/libopencv_videoio411.dll
LIBS += C:/OpenCV411_MinGW_build/x64/mingw/bin/libopencv_calib3d411.dll
LIBS += C:/OpenCV411_MinGW_build/x64/mingw/bin/libopencv_features2d411.dll
