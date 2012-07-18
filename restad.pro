#-------------------------------------------------
# This file is part of the Restad project
# https://sourceforge.net/projects/restad/
#-------------------------------------------------

include(global.pri)

CONFIG    += ordered
TEMPLATE   = subdirs

release {
CONFIG   -= debug # forces qt to ignore debug build, to ignore tests
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

release {
    unix {
        preparser_install.path  = /usr/bin
        indexer_install.path    = /usr/bin
    }
    preparser_install.files = bin/restad-preparser
    indexer_install.files   = bin/restad-indexer
    INSTALLS += preparser_install indexer_install
}
