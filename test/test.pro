#-------------------------------------------------
# This file is part of the Restad project
# https://sourceforge.net/projects/restad/
#-------------------------------------------------

TEMPLATE = app
TARGET   = test

include(../global.pri)
include(../src.pri)

# CuteTest
unix {
CUTE_TEST_INC       = /home/yoann/cutetest/include
CUTE_TEST_LIB_DIR   = /home/yoann/cutetest/lib
CUTE_TEST_LIB       = CuteTestd
}

!isEmpty(CUTE_TEST_INC) { INCLUDEPATH += $$CUTE_TEST_INC }
!isEmpty(CUTE_TEST_LIB_DIR) { LIBS += -L$$CUTE_TEST_LIB_DIR }
LIBS += -l$$CUTE_TEST_LIB

INCLUDEPATH += $$DIR_SRC

SOURCES += test.cpp

HEADERS += \
    global.hpp \
    test_optionmanager.hpp \
    test_global.hpp \
    test_commands.hpp
