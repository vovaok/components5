TEMPLATE = subdirs

SUBDIRS += \
    commlib\
    megaWidgets\
    onb\
    panel3d\
    usbhid\
    simplenet

win32 {
    SUBDIRS += usbhid
}

DISTFILES += build.pri \
             component.pri \
             dir.pri
