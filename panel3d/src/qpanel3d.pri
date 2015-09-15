QT += opengl

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/mesh

CONFIG(debug, debug|release) {
    QMAKE_LIBDIR += $$PWD/build/debug/lib
    LIBS += -lqpanel3dd
} else {
    QMAKE_LIBDIR += $$PWD/build/release/lib
    LIBS += -lqpanel3d
}

LIBS += -lopengl32 -lglu32
