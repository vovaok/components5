QT += opengl

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/mesh

COMPONENT_NAME = "panel3d"

include(../component.pri)

LIBS += -lopengl32 -lglu32
