# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Add-in.
# ------------------------------------------------------

QT += core xml
QT -= gui

include(../common.pri)

INCLUDEPATH += . .. ../..

TARGET = STLib
TEMPLATE = lib
CONFIG += staticlib

SOURCES += *.cpp
HEADERS += *.h

LIBS    += -lvatlib2

DESTDIR = ../../lib

unix {
    QMAKE_CXXFLAGS += -std=c++0x
}
