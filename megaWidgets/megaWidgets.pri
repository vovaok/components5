INCLUDEPATH += $$PWD

android {
    CONFIG(debug, debug|release) {
        QMAKE_LIBDIR += $$PWD/bin/android
        LIBS += -lmegaWidgetsd
    } else {
        QMAKE_LIBDIR += $$PWD/bin/android
        LIBS += -lmegaWidgets
    }
}
win32 {
    CONFIG(debug, debug|release) {
        QMAKE_LIBDIR += $$PWD/../bin/megaWidgets/debug
        LIBS += -lmegaWidgets
    } else {
        QMAKE_LIBDIR += $$PWD/../bin/megaWidgets/release
        LIBS += -lmegaWidgets
    }
}

