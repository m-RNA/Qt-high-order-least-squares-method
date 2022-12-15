TEMPLATE = app
CONFIG +=  c++11
#console
QT       += core gui printsupport serialport

INCLUDEPATH += D:\eigen-3.3.8 D:\Qt\ThirdPartyLib\qcustomplot

SOURCES += \
        D:/Qt/ThirdPartyLib/qcustomplot/qcustomplot.cpp \
        customchart.cpp \
        leastsquare.cpp \
        main.cpp

FORMS += \
    customchart.ui \
    leastsquare.ui

HEADERS += \
    D:/Qt/ThirdPartyLib/qcustomplot/qcustomplot.h \
    customchart.h \
    leastsquare.h
