INCLUDEPATH += $$PWD/src

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
        QMAKE_LIBDIR += $$PWD/bin
        LIBS += -lcommlibd
    } else {
        QMAKE_LIBDIR += $$PWD/bin
        LIBS += -lcommlib
    }
}

