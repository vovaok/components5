QT       += network
QT       -= gui

TEMPLATE = lib
CONFIG += staticlib c++11
TARGET = onb

include(../build.pri)

android {
CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/android-bin
    TARGET = onbd
} else {
    DESTDIR = $$PWD/android-bin
    TARGET = onb
}
}

win32 {
    COMPONENTS = ../
    include($$COMPONENTS/commlib/commlib.pri)
    include($$COMPONENTS/usbhid/usbhid.pri)
}

OBJNET_DIR = $$PWD/../../iar/components/stm32++/src/objnet

INCLUDEPATH += $$OBJNET_DIR
INCLUDEPATH += $$OBJNET_DIR/../core

SOURCES += \
    objnetvirtualinterface.cpp \
    objnetvirtualserver.cpp \
    $$OBJNET_DIR/objnetCommonNode.cpp \
    $$OBJNET_DIR/objnetmaster.cpp \
    $$OBJNET_DIR/objnetInterface.cpp \
    $$OBJNET_DIR/objnetdevice.cpp \
    $$OBJNET_DIR/objnetnode.cpp \
    $$OBJNET_DIR/objectinfo.cpp \
    $$OBJNET_DIR/objnetmsg.cpp \
    usbonbinterface.cpp

HEADERS += \
    objnetvirtualinterface.h \
    objnetvirtualserver.h \
    $$OBJNET_DIR/objnetCommonNode.h \
    $$OBJNET_DIR/objnetmaster.h \
    $$OBJNET_DIR/objnetInterface.h \
    $$OBJNET_DIR/objnetmsg.h \
    $$OBJNET_DIR/objnetcommon.h \
    $$OBJNET_DIR/objnetdevice.h \
    $$OBJNET_DIR/objnetnode.h \
    $$OBJNET_DIR/objectinfo.h \
    $$OBJNET_DIR/../core/closure.h \
    $$OBJNET_DIR/../core/closure_impl.h \
    usbonbinterface.h

win32 {
SOURCES += \
    serialcaninterface.cpp
HEADERS += \
    serialcaninterface.h
}

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    onb.pri
