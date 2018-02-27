INCLUDEPATH += $$PWD

QT += serialport

android {
    CONFIG(debug, debug|release) {
        QMAKE_LIBDIR += $$PWD/commlib/android/bin
        LIBS += -lcommlibd
    } else {
        QMAKE_LIBDIR += $$PWD/commlib/android/bin
        LIBS += -lcommlib
    }
}
win32 {
    CONFIG(debug, debug|release) {
        QMAKE_LIBDIR += $$PWD/../bin/commlib/debug
        LIBS += -lcommlib
    } else {
        QMAKE_LIBDIR += $$PWD/../bin/commlib/release
        LIBS += -lcommlib
    }
}

