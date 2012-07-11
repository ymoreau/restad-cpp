#-------------------------------------------------
# This file is part of the Restad project
# https://sourceforge.net/projects/restad/
#-------------------------------------------------

TEMPLATE = app
TARGET   = preparser

include(../global.pri)
include(../src.pri)

VERSION = 0.1.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

SOURCES += commands/preparser_main.cpp

