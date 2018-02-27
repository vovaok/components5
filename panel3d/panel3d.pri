QT += opengl

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/mesh

win32 {
    CONFIG(debug, debug|release) {
        QMAKE_LIBDIR += $$PWD/../bin/panel3d/debug
        LIBS += -lpanel3d
    } else {
        QMAKE_LIBDIR += $$PWD/../bin/panel3d/release
        LIBS += -lpanel3d
    }
} else {
    message("Only win32 supported.. alas")
}

LIBS += -lopengl32 -lglu32
