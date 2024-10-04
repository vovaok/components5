INCLUDEPATH += $$PWD

QT += network

include($$PWD/../commlib/commlib.pri)

win32 {
    include($$PWD/../usbhid/usbhid.pri)
}

COMPONENT_NAME = "onb"

OBJNET_DIR = $$PWD/../../../iar/components/stm32++/src/objnet

INCLUDEPATH += $$OBJNET_DIR
INCLUDEPATH += $$OBJNET_DIR/..

include(../component.pri)
