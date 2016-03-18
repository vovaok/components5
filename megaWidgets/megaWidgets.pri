INCLUDEPATH += $$PWD

android {
    CONFIG(debug, debug|release) {
        QMAKE_LIBDIR += $$PWD/megaWidgets-android-debug/build/debug
        LIBS += -lmegaWidgetsd
    } else {
        QMAKE_LIBDIR += $$PWD/megaWidgets-android-release/build/release
        LIBS += -lmegaWidgets
    }
}
win32 {
    CONFIG(debug, debug|release) {
        QMAKE_LIBDIR += $$PWD/bin
        LIBS += -lmegaWidgetsd
    } else {
        QMAKE_LIBDIR += $$PWD/bin
        LIBS += -lmegaWidgets
    }
}

