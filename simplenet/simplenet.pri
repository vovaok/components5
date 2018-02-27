INCLUDEPATH += $$PWD

QT += serialport

android {
    CONFIG(debug, debug|release) {
        QMAKE_LIBDIR += $$PWD/x3
        LIBS += -lsimplenet
    } else {
        QMAKE_LIBDIR += $$PWD/x3
        LIBS += -lsimplenet
    }
}
win32 {
    CONFIG(debug, debug|release) {
        QMAKE_LIBDIR += $$PWD/../bin/simplenet/debug
        LIBS += -lsimplenet
    } else {
        QMAKE_LIBDIR += $$PWD/../bin/simplenet/release
        LIBS += -lsimplenet
    }
}

