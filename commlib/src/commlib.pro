QT       += widgets network serialport
QT       -= gui

TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++11


DESTDIR = $$PWD/../bin

# чтобы сразу скомпилить и в дебаге, и в релизе:
CONFIG += debug_and_release
CONFIG += build_all

CONFIG(debug, debug|release) {
    TARGET = commlibd
} else {
    TARGET = commlib
}

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
    ../commlib.pri \
    Doxyfile
