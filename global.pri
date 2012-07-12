#-------------------------------------------------
# This file is part of the Restad project
# https://sourceforge.net/projects/restad/
#-------------------------------------------------

DIR_ROOT    = $$PWD
DIR_SRC     = $$DIR_ROOT/src
DESTDIR     = $$DIR_ROOT/bin
OBJECTS_DIR = $$DIR_ROOT/bin
MOC_DIR     = $$DIR_ROOT/bin

DEFINES += BUILD_DIR=\\\"$$PWD\\\" # This is for CuteTest

# General config
QT += core xml
QT -= gui
CONFIG  += console
CONFIG  -= app_bundle

INCLUDEPATH += /usr/include/postgresql

LIBS += -lpq
