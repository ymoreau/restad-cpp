#-------------------------------------------------
# This file is part of the Restad project
# https://sourceforge.net/projects/restad/
#-------------------------------------------------

TEMPLATE = app
TARGET   = restad-indexer

include(../global.pri)
include(../src.pri)

VERSION = 0.1.1
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

SOURCES += commands/indexer_main.cpp
