INCLUDEPATH += $$PWD/src

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
        QMAKE_LIBDIR += $$PWD/megaWidgets-win32-debug/build/debug
        LIBS += -lmegaWidgetsd
    } else {
        QMAKE_LIBDIR += $$PWD/megaWidgets-win32-release/build/release
        LIBS += -lmegaWidgets
    }
}

