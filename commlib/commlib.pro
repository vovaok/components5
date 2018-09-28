QT       += widgets network serialport
QT       -= gui
TEMPLATE = lib
CONFIG += staticlib c++11
TARGET = commlib

include(../build.pri)

SOURCES += seriallink.cpp \
    serialframe.cpp \
    serialcan.cpp \
    serialportwidget.cpp

HEADERS += seriallink.h \
    serialframe.h \
    serialcan.h \
    serialportwidget.h \
    uartinterface.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

OTHER_FILES += \
    commlib.pri \
    Doxyfile
