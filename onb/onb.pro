#-------------------------------------------------
#
# Project created by QtCreator 2016-06-20T09:39:15
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = onb
TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++11
# чтобы сразу скомпилить и в дебаге, и в релизе:
CONFIG += debug_and_release
CONFIG += build_all

CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/bin
    TARGET = onbd
} else {
    DESTDIR = $$PWD/bin
    TARGET = onb
}

COMPONENTS = d:/projects/qt/components5
include($$COMPONENTS/commlib/commlib.pri)
include($$COMPONENTS/usbhid/usbhid.pri)

PROJ_DIR = d:/projects
OBJNET_DIR = $$PROJ_DIR/iar/components/stm32++/src/objnet

INCLUDEPATH += $$OBJNET_DIR
INCLUDEPATH += $$OBJNET_DIR/../core

SOURCES += \
    objnetvirtualinterface.cpp \
    objnetvirtualserver.cpp \
    serialcaninterface.cpp \
    usbhidonbinterface.cpp \
    $$OBJNET_DIR/objnetCommonNode.cpp \
    $$OBJNET_DIR/objnetmaster.cpp \
    $$OBJNET_DIR/objnetInterface.cpp \
    $$OBJNET_DIR/objnetdevice.cpp \
    $$OBJNET_DIR/objnetnode.cpp \
    $$OBJNET_DIR/objectinfo.cpp \
    $$OBJNET_DIR/objnetmsg.cpp

HEADERS += \
    objnetvirtualinterface.h \
    objnetvirtualserver.h \
    serialcaninterface.h \
    usbhidonbinterface.h \
    $$OBJNET_DIR/objnetCommonNode.h \
    $$OBJNET_DIR/objnetmaster.h \
    $$OBJNET_DIR/objnetInterface.h \
    $$OBJNET_DIR/objnetmsg.h \
    $$OBJNET_DIR/objnetcommon.h \
    $$OBJNET_DIR/objnetdevice.h \
    $$OBJNET_DIR/objnetnode.h \
    $$OBJNET_DIR/objectinfo.h \
    $$OBJNET_DIR/../core/closure.h \
    $$OBJNET_DIR/../core/closure_impl.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    onb.pri
