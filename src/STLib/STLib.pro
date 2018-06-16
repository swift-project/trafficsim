QT += core xml
QT -= gui

include(../../common.pri)

INCLUDEPATH += . .. ../..

TARGET = STLib
TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++14

SOURCES += *.cpp
HEADERS += *.h

LIBS    += -lvatlib

DESTDIR = $$BuildRoot/lib
