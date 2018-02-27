#-------------------------------------------------
#
# Project created by QtCreator 2013-09-20T00:03:04
#
#-------------------------------------------------

QT       -= gui

win32-msvc*: { #FOR VisualC++
    INCLUDEPATH += "C:/Program Files (x86)/Windows Kits/10/Include/10.0.10240.0/ucrt"
    LIBS += -L"C:/Program Files (x86)/Windows Kits/10/Lib/10.0.10240.0/ucrt/x86"
}

include(../build.pri)

TARGET = usbhid
TEMPLATE = lib
CONFIG += staticlib

#CONFIG(debug, debug|release) {
#    DESTDIR = $$PWD/bin
#    TARGET = usbhidd
#} else {
#    DESTDIR = $$PWD/bin
#    TARGET = usbhid
#}

LIBS        += -lsetupapi

SOURCES += usbhid.cpp \
    usbhidthread.cpp

win32 {
    SOURCES += hid_windows.c
}
#unix {
#    SOURCES += hid_linux.c
#    message(xep)
#}

HEADERS += usbhid.h \
    hidapi.h \
    usbhidthread.h
unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

OTHER_FILES += \
    usbhid.pri
