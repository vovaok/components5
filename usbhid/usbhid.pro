#-------------------------------------------------
#
# Project created by QtCreator 2013-09-20T00:03:04
#
#-------------------------------------------------

QT       -= gui

#TARGET = usbhid
TEMPLATE = lib
CONFIG += staticlib
# чтобы сразу скомпилить и в дебаге, и в релизе:
CONFIG += debug_and_release
CONFIG += build_all

CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/bin
    TARGET = usbhidd
} else {
    DESTDIR = $$PWD/bin
    TARGET = usbhid
}

LIBS        += -lsetupapi

SOURCES += usbhid.cpp \
    hid.c

HEADERS += usbhid.h \
    hidapi.h
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
