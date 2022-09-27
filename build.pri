# чтобы сразу скомпилить и в дебаге, и в релизе:
CONFIG += debug_and_release
CONFIG += build_all
CONFIG += c++11

COMPONENT_NAME = $${TARGET}

include(dir.pri)

DESTDIR = $$LIBDIR
