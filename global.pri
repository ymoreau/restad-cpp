#-------------------------------------------------
# This file is part of the Restad project
# https://sourceforge.net/projects/restad/
#-------------------------------------------------

DIR_ROOT = $$PWD
DIR_SRC  = $$DIR_ROOT/src
DEFINES += BUILD_DIR=\\\"$$PWD\\\" # This is for CuteTest

# General config
QT += core xml
QT -= gui
CONFIG  += console
CONFIG  -= app_bundle

INCLUDEPATH += /usr/include/postgresql

LIBS += -lpq
