TEMPLATE = subdirs
SUBDIRS += STLib \
    STExport \
    STd

include(common.pri)

win32 {
    CONFIG(debug, debug|release): DLL_DEBUG_SUFFIX = d
    qt5_target.path = ../dist/bin
    qt5_target.files *= $$[QT_INSTALL_BINS]/Qt5Core$${DLL_DEBUG_SUFFIX}.dll
} else:macx {
    # To be implemented
    qt5_target.path = ../dist/lib
} else:unix: {
    # To be implemented
    qt5_target.path = ../dist/lib
    QT5_LIBRARY_DIR = $$[QT_INSTALL_LIBS]
}
INSTALLS += qt5_target

win32 {
    vatlib_target.path = ../dist/bin
    vatlib_target.files *= $$_VATLIB_ROOT/lib/*.dll
} else {
    # To be implemented
    vatlib_target.path = ../dist/lib
}
INSTALLS += vatlib_target

win32-g++ {
    vc_runtime_target.files *= $$[QT_INSTALL_BINS]/libgcc_s_dw2-1.dll
    vc_runtime_target.files *= $$[QT_INSTALL_BINS]/libwinpthread-1.dll
    vc_runtime_target.files *= $$[QT_INSTALL_BINS]/libstdc++-6.dll
    vc_runtime_target.path *= ../dist/bin
    INSTALLS += vc_runtime_target
}

scenarios_target.path = ../dist/Logs
scenarios_target.files = ../Logs/*.xml
INSTALLS += scenarios_target
