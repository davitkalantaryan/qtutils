#
# file:		some_qt_tests.pro
# path:		prj/tests/some_qt_tests_qt/some_qt_tests.pro
# created on:	2023 Jan 21
# creatd by:	Davit Kalantaryan (davit.kalantaryan@desy.de)
#

message("!!! $${_PRO_FILE_}")

TEMPLATE = app

include ( "$${PWD}/../../common/common_qt/flagsandsys_common.pri" )
DESTDIR     = "$${artifactRoot}/sys/$${CODENAME}/$$CONFIGURATION/test"


QT += widgets
QT += network
#QT += sql
QT += websockets

CONFIG += c++17
#QTPLUGIN += qtvirtualkeyboardplugin


SOURCES += "$${qtutilsRepoRoot}/src/tests/main_networkacccessmanager_test01.cpp"
SOURCES += "$${qtutilsRepoRoot}/src/core/qtutils_core_networkaccessmanager.cpp"
SOURCES += "$${qtutilsRepoRoot}/src/core/qtutils_core_logger.cpp"

HEADERS += "$${qtutilsRepoRoot}/include/qtutils/core/networkaccessmanagerqu.hpp"
