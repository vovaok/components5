PLATFORM = "x86"
win32-msvc*:contains(QMAKE_TARGET.arch, x86_64): {
    CONFIG += IS_X64
    DEFINES += IS_X64
    PLATFORM = "x64"
}

unix {
    PLATFORM = $$QMAKE_HOST.arch
}

CONFIG += skip_target_version_ext

#Define DEBUG constant
CONFIG(debug,debug|release)
{
    IS_DEBUG = true
}

COMPILER = ''

win32: {
    win32-msvc* {
        MSVC_VER = $$(VisualStudioVersion)
        equals(MSVC_VER, 10.0){ COMPILER = 'msvc2008' }
        equals(MSVC_VER, 11.0){ COMPILER = 'msvc2010' }
        equals(MSVC_VER, 12.0){ COMPILER = 'msvc2012' }
        equals(MSVC_VER, 13.0){ COMPILER = 'msvc2013' }
        equals(MSVC_VER, 14.0){ COMPILER = 'msvc2015' }
        equals(MSVC_VER, 15.0){ COMPILER = 'msvc2017' }
    }
    else {
        COMPILER = 'mingw'
    }
}
else { COMPILER = $$QMAKE_CXX }


LIBDIR = "$$PWD/bin"

win32:contains(QMAKE_TARGET.arch, x86_64)
{
    LIBDIR = "$$PWD/bin64"
}
win32-msvc*: {
    LIBDIR = "$$PWD/bin-msvc"
}
android {
    LIBDIR = "$$PWD/bin-android"
}

ROOTBIN = $$LIBDIR
LIBDIR = $$join(LIBDIR,,,/$${COMPONENT_NAME})

DIRS = $${ROOTBIN} \
       $${LIBDIR} \
       $${LIBDIR}/debug \
       $${LIBDIR}/release

#message($$COMPONENT_NAME)
#message(DIRS $$DIRS)

CONFIG(debug, debug|release): LIBDIR = $$join(LIBDIR,,,/debug)
else:                         LIBDIR = $$join(LIBDIR,,,/release)

DEPENDPATH += $${LIBDIR}

for(DIR, DIRS) {
     mkcommands += $$OUT_PWD/$$DIR
}

createDirs.commands = $(MKDIR) $$mkcommands
first.depends *= createDirs
QMAKE_EXTRA_TARGETS *= createDirs

QMAKE_TARGET_COMPANY = "Neurobotics"
QMAKE_TARGET_COPYRIGHT = "Copyright (C) 2008-2018 Neurobotics, Ltd <info@neurobotics.ru>"


#message(LD $$LIBDIR)
