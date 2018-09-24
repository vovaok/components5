
android {
    LIBDIR_PREFIX = $$PWD/bin-android
}
win32-msvc* {
    LIBDIR_PREFIX = $$PWD/Desktop_Qt_5_8_0_MSVC2015_32bit
}
win32-mingw {
    LIBDIR_PREFIX = $$PWD/bin
}

CONFIG(debug, debug|release) {
    QMAKE_LIBDIR += $$LIBDIR_PREFIX/$$COMPONENT_NAME/debug
    LIBS += -l$$COMPONENT_NAME
} else {
    QMAKE_LIBDIR += $$LIBDIR_PREFIX/$$COMPONENT_NAME/release
    LIBS += -l$$COMPONENT_NAME
}
