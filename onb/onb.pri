INCLUDEPATH += $$PWD

QT += network

win32 {
    include($$PWD/../usbhid/usbhid.pri)
    include($$PWD/../commlib/commlib.pri)
}

COMPONENT_NAME = "onb"

OBJNET_DIR = $$PWD/../../iar/components/stm32++/src/objnet

INCLUDEPATH += $$OBJNET_DIR
INCLUDEPATH += $$OBJNET_DIR/../core

include(../component.pri)
