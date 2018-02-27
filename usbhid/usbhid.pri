INCLUDEPATH += $$PWD

android {
    CONFIG(debug, debug|release) {
        QMAKE_LIBDIR += $$PWD/x3
        LIBS += -lusbhid
    } else {
        QMAKE_LIBDIR += $$PWD/x3
        LIBS += -lusbhid
    }
}
win32 {
    CONFIG(debug, debug|release) {
        QMAKE_LIBDIR += $$PWD/../bin/usbhid/debug
        LIBS += -lusbhid
    } else {
        QMAKE_LIBDIR += $$PWD/../bin/usbhid/release
        LIBS += -lusbhid
    }
}

LIBS += -lsetupapi
