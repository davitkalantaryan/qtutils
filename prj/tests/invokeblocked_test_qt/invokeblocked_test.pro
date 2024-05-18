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

DEFINES += QTUTILS_HTTP_SERVER_TOOLS


QT += widgets
QT += network
#QT += sql
QT += websockets
QT += httpserver

CONFIG += c++17
#QTPLUGIN += qtvirtualkeyboardplugin


#SOURCES += $$files($${qtutilsRepoRoot}/src/tests/some_qt_tests/*.cpp,true)
SOURCES += "$${qtutilsRepoRoot}/src/tests/main_invokeblocked_test.cpp"
SOURCES += "$${qtutilsRepoRoot}/src/core/qtutils_core_clsinvokeblocked.cpp"
HEADERS += "$${qtutilsRepoRoot}/include/qtutils/core/clsinvokeblocked.hpp"
