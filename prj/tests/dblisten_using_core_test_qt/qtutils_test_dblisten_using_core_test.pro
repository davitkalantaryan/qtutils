#
# repo:		qtutils
# file:		qtutils_test_dblisten_using_core_test.pro
# path:		prj/tests/dblisten_using_core_test_qt/qtutils_test_dblisten_test.pro
# created on:	2026 Sep 26
# created by:	Davit Kalantaryan (davit.kalantaryan@desy.de)
#

message("!!! $${_PRO_FILE_}")

include ( "$${PWD}/../../common/common_qt/flagsandsys_common.pri" )
DESTDIR = "$${ArifactFinal}/test"

QT += sql

SOURCES	+= "$${qtutilsRepoRoot}/src/tests/main_qtutils_test_dblisten_using_core_test.cpp"
SOURCES += "$${qtutilsRepoRoot}/src/core/qtutils_core_sql.cpp"
