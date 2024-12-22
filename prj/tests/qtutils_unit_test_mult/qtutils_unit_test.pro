#
# file:			googletest_getter.pro
# path:			prj/tests/googletest_mult/googletest_getter.pro
# created on:		2021 Mar 27
# created by:		Davit Kalantaryan
#

message("!!! $${_PRO_FILE_}")

include ( "$${PWD}/../../common/common_qt/flagsandsys_common_private.pri" )
DESTDIR = "$${ArifactFinal}/test"

QT += svg
QT += charts
QT += sql
QT += widgets
QT += network
QT += websockets
QT += httpserver

win32{
} else {
	LIBS += -pthread
}

DEFINES += CINTERNAL_UNIT_TEST_USE_GTEST_LIKE_MACROSES

SRC_DIR=$${qtutilsRepoRoot}/src/tests/unit_test


SOURCES += $$files($${SRC_DIR}/*.cpp,true)
#SOURCES += $$files($${repoRootPath}/src/core/*.cpp,true)

SOURCES +=  \
    "$${cinternalRepoRoot}/src/core/cinternal_core_logger.c"				\
    "$${cinternalRepoRoot}/src/core/cinternal_core_unit_test_checks.c"			\
    "$${cinternalRepoRoot}/src/core/cinternal_core_unit_test_main.c"

#HEADERS =  $$files($${qtutilsRepoRoot}/include/*.h,true)
#HEADERS += $$files($${qtutilsRepoRoot}/include/*.hpp,true)
HEADERS += $$files($${SRC_DIR}/*.h,true)
HEADERS += $$files($${SRC_DIR}/*.hpp,true)
OTHER_FILES += $$files($${PWD}/*.Makefile,false)
