include(dir.pri)
#message(LIBDIR $$LIBDIR)
QMAKE_LIBDIR += $${LIBDIR}
LIBS += -l$$COMPONENT_NAME
