# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Add-in.
# ------------------------------------------------------

QT += core xml
QT -= gui

include(../common.pri)

INCLUDEPATH += . .. ../..

TARGET = STExport
TEMPLATE = app
CONFIG += console

SOURCES += *.cpp
HEADERS += *.h

LIBS    += -L../../lib -lSTLib -lvatlib2

DESTDIR = ../../bin

unix {
    QMAKE_CXXFLAGS += -std=c++0x
}
