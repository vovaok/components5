QT       -= gui
TARGET = usbhid
TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++11

win32-msvc*: { #FOR VisualC++
    INCLUDEPATH += "C:/Program Files (x86)/Windows Kits/10/Include/10.0.10240.0/ucrt"
    LIBS += -L"C:/Program Files (x86)/Windows Kits/10/Lib/10.0.10240.0/ucrt/x86"
}

include(../build.pri)



#CONFIG(debug, debug|release) {
#    DESTDIR = $$PWD/bin
#    TARGET = usbhidd
#} else {
#    DESTDIR = $$PWD/bin
#    TARGET = usbhid
#}



SOURCES += usbhid.cpp \
    usbhidthread.cpp \
    usbdeviceenumerator.cpp

win32: {
    SOURCES += hid_windows.c
    LIBS        += -lsetupapi
}
unix: {
    SOURCES += hid_linux.c
}

HEADERS += usbhid.h \
    hidapi.h \
    usbhidthread.h \
    usbdeviceenumerator.h
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
