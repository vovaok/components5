INCLUDEPATH += $$PWD

QT += network

win32 {
    include($$PWD/../usbhid/usbhid.pri)
    include($$PWD/../commlib/commlib.pri)
}

PROJ_DIR = d:/projects
OBJNET_DIR = $$PROJ_DIR/iar/components/stm32++/src/objnet

INCLUDEPATH += $$OBJNET_DIR
INCLUDEPATH += $$OBJNET_DIR/../core

android {
    CONFIG(debug, debug|release) {
        QMAKE_LIBDIR += $$PWD/../bin-android/onb/debug
        LIBS += -lonb
    } else {
        QMAKE_LIBDIR += $$PWD/../bin-android/onb/release
        LIBS += -lonb
    }
}
win32 {
    CONFIG(debug, debug|release) {
        QMAKE_LIBDIR += $$PWD/../bin/onb/debug
        LIBS += -lonb
    } else {
        QMAKE_LIBDIR += $$PWD/../bin/onb/release
        LIBS += -lonb
    }
}
