INCLUDEPATH += $$PWD

QMAKE_LIBDIR += $$PWD/bin
LIBS += -lusbhid

#CONFIG(debug, debug|release) {
#    LIBS += -lusbhidd
#} else {
#    LIBS += -lusbhid
#}

LIBS += -lsetupapi
