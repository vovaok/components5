QT += opengl

INCLUDEPATH += $$PWD/src
INCLUDEPATH += $$PWD/src/mesh

win32 {
    CONFIG(debug, debug|release) {
        QMAKE_LIBDIR += $$PWD/build-mingw/bin
        LIBS += -lqpanel3dd
    } else {
        QMAKE_LIBDIR += $$PWD/build-mingw/bin
        LIBS += -lqpanel3d
    }
} else {
    message("Only win32 supported.. alas")
}



LIBS += -lopengl32 -lglu32
