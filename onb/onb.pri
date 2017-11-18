INCLUDEPATH += $$PWD

QT += network

message("PIZDEC1")

win32 {
    include(../usbhid/usbhid.pri)
    include(../commlib/commlib.pri)
message("PIZDEC")
}

message("PIZDEC2")

PROJ_DIR = d:/projects
OBJNET_DIR = $$PROJ_DIR/iar/components/stm32++/src/objnet

INCLUDEPATH += $$OBJNET_DIR
INCLUDEPATH += $$OBJNET_DIR/../core

win32 {
    QMAKE_LIBDIR += $$PWD/bin
}
android {
    QMAKE_LIBDIR += $$PWD/android-bin
}

CONFIG(debug, debug|release) {
    LIBS += -lonbd
} else {
    LIBS += -lonb
}

message("PIZDEC3")
