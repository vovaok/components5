#-------------------------------------------------
#
# Project created by QtCreator 2013-08-01T15:09:09
#
#-------------------------------------------------

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = lib
CONFIG += staticlib
# чтобы сразу скомпилить и в дебаге, и в релизе:
CONFIG += debug_and_release
CONFIG += build_all

DESTDIR = $$PWD/bin

CONFIG(debug, debug|release) {
    TARGET = megaWidgetsd
} else {
    TARGET = megaWidgets
}

SOURCES += led.cpp \
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

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

OTHER_FILES += \
    ../megaWidgets.pri
