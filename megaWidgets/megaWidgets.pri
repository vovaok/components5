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
        QMAKE_LIBDIR += $$PWD/bin
        LIBS += -lmegaWidgetsd
    } else {
        QMAKE_LIBDIR += $$PWD/bin
        LIBS += -lmegaWidgets
    }
}

