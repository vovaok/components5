INCLUDEPATH += $$PWD

COMPONENT_NAME = "usbhid"

include(../component.pri)

LIBS += -lsetupapi
