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
CONFIG += c++11

SOURCES += *.cpp
HEADERS += *.h

LIBS    += -L../../lib -lSTLib -lvatlib

DESTDIR = ../../bin

target.path = ../../dist/bin
INSTALLS += target
