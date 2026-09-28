#
# repo:			qtutils
# file:			qtutils_test_dev_all_files01.pro
# path:			prj/tests/dev_all_files01_qt/any_quick_test.pro
# created on:	2026 Sep 13
# created by:	Davit Kalantaryan (davit.kalantaryan@desy.de)
#

message("!!! $${_PRO_FILE_}")

include ( "$${PWD}/../../common/common_qt/flagsandsys_common.pri" )
include ( "$${qtutilsRepoRoot}/prj/common/common_qt/use_qtkeychain_cmn_no_source.pri" )
DESTDIR = "$${ArifactFinal}/test"

DEFINES += QTUTILS_HTTP_SERVER_TOOLS

QT += sql
QT += widgets
QT += network
QT += httpserver

SOURCES += "$${qtutilsRepoRoot}/src/tests/main_qtutils_test_dev_all_files01.cpp"
SOURCES += $$files($${qtutilsRepoRoot}/src/core/*.cpp,true)
SOURCES += $$files($${qtutilsRepoRoot}/src/ui/*.cpp,true)
SOURCES += $$files($${qtutilsRepoRoot}/src/utils/*.cpp,true)
SOURCES += "$${cinternalRepoRoot}/src/core/cinternal_core_logger.c"

HEADERS += $$files($${qtutilsRepoRoot}/include/*.hpp,true)
HEADERS += $$files($${qtutilsRepoRoot}/include/*.h,true)
HEADERS += $$files($${qtutilsRepoRoot}/src/*.hpp,true)
HEADERS += $$files($${qtutilsRepoRoot}/src/*.h,true)
