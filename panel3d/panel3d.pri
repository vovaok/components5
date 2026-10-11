QT += opengl

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/mesh

COMPONENT_NAME = "panel3d"

include(../component.pri)

win32 {
    LIBS += -lopengl32 -lglu32
}
