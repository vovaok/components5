QT += widgets
TARGET = megaWidgets
TEMPLATE = lib
CONFIG += staticlib

include(../build.pri)

##android {
##    DESTDIR = $$PWD/bin/android
##}
#win32 {
#    DESTDIR = $$PWD/bin
#}

#CONFIG(debug, debug|release) {
#    TARGET = megaWidgetsd
#} else {
#    TARGET = megaWidgets
#}

SOURCES += led.cpp \
    canvaswidget.cpp \
    graphwidget.cpp \
    labeledled.cpp \
    toggleswitch.cpp \
    segmentindicator.cpp \
    scalerindicator.cpp \
    sbutton.cpp \
    rotaryswitch.cpp \
    potentiometer.cpp \
    ledcontainer.cpp \
    joystickwidget.cpp \
    colorpicker.cpp \
    button.cpp

HEADERS += led.h \
    canvaswidget.h \
    graphwidget.h \
    labeledled.h \
    toggleswitch.h \
    segmentindicator.h \
    scalerindicator.h \
    sbutton.h \
    rotaryswitch.h \
    potentiometer.h \
    ledcontainer.h \
    joystickwidget.h \
    colorpicker.h \
    button.h

#unix:!symbian {
#    maemo5 {
#        target.path = /opt/usr/lib
#    } else {
#        target.path = /usr/lib
#    }
#    INSTALLS += target
#}

OTHER_FILES += \
    ../megaWidgets.pri

DISTFILES += \
    megawidgets.pri
