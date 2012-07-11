#-------------------------------------------------
# This file is part of the Restad project
# https://sourceforge.net/projects/restad/
#-------------------------------------------------

include(global.pri)

CONFIG    += ordered
TEMPLATE   = subdirs

CONFIG   -= release # will be ignored if building in release
release {
CONFIG   -= debug # forces qt to ignore debug build
}

debug {
CONFIG   += warn_on
}

# Targets
SUBDIRS         = preparser indexer
preparser.file  = src/preparser.pro
indexer.file    = src/indexer.pro

debug {
SUBDIRS     += test
test.file    = test/test.pro
}

SOURCES += src/doc-main-page.cpp
