QT       += widgets network
QT       -= gui

TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++11

include(../build.pri)


OTHER_FILES += \
    simplenet.pri

HEADERS += \
    netconnector.h \
    simpleserver.h \
    simplesocket.h \
    commontypes.h

SOURCES += \
    netconnector.cpp \
    simpleserver.cpp \
    simplesocket.cpp
