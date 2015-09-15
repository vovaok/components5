# -------------------------------------------------
# Project created by QtCreator 2011-09-26T09:54:44
# -------------------------------------------------
QT += opengl
TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++11
# чтобы сразу скомпилить и в дебаге, и в релизе:
CONFIG += debug_and_release
CONFIG += build_all

CONFIG(debug, debug|release) {
    MOC_DIR = debug
    OBJECTS_DIR = debug
    DESTDIR = bin
    TARGET = qpanel3dd
} else {
    MOC_DIR = release
    OBJECTS_DIR = release
    DESTDIR = bin
    TARGET = qpanel3d
}

INCLUDEPATH += mesh

LIBS += -lopengl32
LIBS += -lglu32

SOURCES += qpanel3d.cpp \
    primitive3d.cpp \
    object3d.cpp \
    mesh3d.cpp \
    light3d.cpp \
    camera3d.cpp \
    mesh3dcache.cpp \
    mesh/vrml2types.cpp \
    mesh/vrml2parser.cpp \
    mesh/mesh.cpp \
    graph3d.cpp \
    pointcloud3d.cpp \
    texture.cpp \
    graph2d.cpp \
    dynamicobject3d.cpp \
    dynamicmodel.cpp \
    terrain3d.cpp
HEADERS += qpanel3d.h \
    primitive3d.h \
    panel3d.h \
    object3d.h \
    mesh3d.h \
    light3d.h \
    camera3d.h \
    mesh3dcache.h \
    mesh/vrml2types.h \
    mesh/vrml2parser.h \
    mesh/mesh.h \
    graph3d.h \
    pointcloud3d.h \
    texture.h \
    graph2d.h \
    dynamicobject3d.h \
    dynamicmodel.h \
    terrain3d.h

OTHER_FILES += \
    ../qpanel3d.pri
