TEMPLATE = app
CONFIG +=  c++11 console

QT       += core gui printsupport serialport opengl


INCLUDEPATH += D:\eigen-3.3.8 D:\Qt\ThirdPartyLib\qcustomplot

SOURCES += \
        D:/Qt/ThirdPartyLib/qcustomplot/qcustomplot.cpp \
        bll_leastssquare.cpp \
        cv_table.cpp \
        fitchart.cpp \
        leastsquare.cpp \
        main.cpp \
        config.c

FORMS += \
    leastsquare.ui

HEADERS += \
    D:/Qt/ThirdPartyLib/qcustomplot/qcustomplot.h \
    bll_leastssquare.h \
    fitchart.h \
    leastsquare.h \
    cv_table.h\
    config.h
DEFINES += QCUSTOMPLOT_USE_OPENGL
LIBS+=-lopengl32 -lglu32

RESOURCES += \
    ../TemperatureSensorCalibration/main.qrc
