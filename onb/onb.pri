INCLUDEPATH += $$PWD

QT += network

win32 {
    include($$PWD/../usbhid/usbhid.pri)
    include($$PWD/../commlib/commlib.pri)
}

COMPONENT_NAME = "onb"

PROJ_DIR = d:/projects
OBJNET_DIR = $$PROJ_DIR/iar/components/stm32++/src/objnet

INCLUDEPATH += $$OBJNET_DIR
INCLUDEPATH += $$OBJNET_DIR/../core

include(../component.pri)
