# Find vatlib library.

# Try to use qmake variable's value.
_VATLIB_ROOT = $$VATLIB_ROOT
isEmpty(_VATLIB_ROOT) {
    message(\"Vatlib Library\" qmake value not detected...)

    # Try to use the system environment value.
    _VATLIB_ROOT = $$(VATLIB_ROOT)
}

isEmpty(_VATLIB_ROOT) {
    message(\"Vatlib library\" environment variable not detected...)
    !build_pass:error("Please set the environment variable `VATLIB_ROOT`. For example, VATLIB_ROOT=c:\\vatlib")
} else {
    message(\"Vatlib library\" detected in VATLIB_ROOT = \"$$_VATLIB_ROOT\")
    INCLUDEPATH += $$_VATLIB_ROOT/include
    LIBS += -L$$_VATLIB_ROOT/lib
}
