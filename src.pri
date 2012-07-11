#-------------------------------------------------
# This file is part of the Restad project
# https://sourceforge.net/projects/restad/
#-------------------------------------------------

include(global.pri)

# Source files
SOURCES += \
    $$DIR_SRC/global/optionmanager.cpp \
    $$DIR_SRC/global/exception.cpp \
    $$DIR_SRC/global/database.cpp \
    $$DIR_SRC/commands/preparser.cpp \
    $$DIR_SRC/commands/indexer.cpp \
    $$DIR_SRC/preparser/preparserdatabase.cpp \
    $$DIR_SRC/parser/indexingprogress.cpp \
    $$DIR_SRC/parser/abstractparser.cpp \
    $$DIR_SRC/parser/globalparserdatabase.cpp \
    $$DIR_SRC/parser/datamanager.cpp \
    $$DIR_SRC/parser/parserfactory.cpp \
    $$DIR_SRC/parser/documentlist.cpp \
    $$DIR_SRC/parser/tagfilter.cpp \
    $$DIR_SRC/parser/xmlparser.cpp \
    $$DIR_SRC/parser/stringfilters.cpp

HEADERS += \
    $$DIR_SRC/global/optionmanager.hpp \
    $$DIR_SRC/global/exception.hpp \
    $$DIR_SRC/global/database.hpp \
    $$DIR_SRC/commands/preparser.hpp \
    $$DIR_SRC/commands/indexer.hpp \
    $$DIR_SRC/preparser/preparserdatabase.hpp \
    $$DIR_SRC/parser/indexingprogress.hpp \
    $$DIR_SRC/parser/abstractparser.hpp \
    $$DIR_SRC/parser/globalparserdatabase.hpp \
    $$DIR_SRC/parser/datamanager.hpp \
    $$DIR_SRC/parser/parserfactory.hpp \
    $$DIR_SRC/parser/documentlist.hpp \
    $$DIR_SRC/parser/tagfilter.hpp \
    $$DIR_SRC/parser/xmlparser.hpp \
    $$DIR_SRC/parser/stringfilters.hpp
