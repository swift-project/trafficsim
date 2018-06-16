QT += core xml
QT -= gui

include(../../common.pri)

INCLUDEPATH += . .. ../..

TARGET = STd
TEMPLATE = app
CONFIG += console
CONFIG += c++14

SOURCES += *.cpp
HEADERS += *.h

LIBS    += -L$$BuildRoot/lib -lSTLib -lvatlib

DESTDIR = $$BuildRoot/bin

target.path = $$BuildRoot/dist/bin
INSTALLS += target
