INCLUDEPATH += $$PWD/src

QT += serialport

android {
    CONFIG(debug, debug|release) {
        QMAKE_LIBDIR += $$PWD/build-commlib-android-debug/debug
        LIBS += -lcommlibd
    } else {
        QMAKE_LIBDIR += $$PWD/build-commlib-android-release/release
        LIBS += -lcommlib
    }
}
win32 {
    CONFIG(debug, debug|release) {
        QMAKE_LIBDIR += $$PWD/build-commlib-mingw/debug
        LIBS += -lcommlibd
    } else {
        QMAKE_LIBDIR += $$PWD/build-commlib-mingw/release
        LIBS += -lcommlib
    }
}

