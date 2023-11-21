#
# file:		qt_sql_test.pro
# path:		prj/tests/qt_sql_test_qt/qt_sql_test.pro
# created on:	2023 Nov 21
# creatd by:	Davit Kalantaryan (davit.kalantaryan@desy.de)
#

message("!!! $${_PRO_FILE_}")

TEMPLATE = app

include ( "$${PWD}/../../common/common_qt/flagsandsys_common.pri" )
DESTDIR     = "$${artifactRoot}/sys/$${CODENAME}/$$CONFIGURATION/test"


QT += sql

CONFIG += c++17


SOURCES += $$files($${qtutilsRepoRoot}/src/tests/qt_sql_test/*.cpp,true)
SOURCES += "$${qtutilsRepoRoot}/src/core/qtutils_core_sqldbwrp.cpp"
HEADERS += $$files($${qtutilsRepoRoot}/src/tests/qt_sql_test/*.hpp,true)
HEADERS += "$${qtutilsRepoRoot}/include/qtutils/core/sql.hpp"
HEADERS += "$${qtutilsRepoRoot}/include/qtutils/core/sqldatabase.hpp"
HEADERS += "$${qtutilsRepoRoot}/include/qtutils/core/sqldbwrp.hpp"
HEADERS += "$${qtutilsRepoRoot}/include/qtutils/core/sqldbwrp_p.hpp"

