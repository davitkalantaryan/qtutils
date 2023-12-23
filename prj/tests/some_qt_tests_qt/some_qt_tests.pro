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


SOURCES += $$files($${qtutilsRepoRoot}/src/tests/some_qt_tests/*.cpp,true)
SOURCES += "$${qtutilsRepoRoot}/src/ui/qtutils_ui_global_functions.cpp"
SOURCES += "$${qtutilsRepoRoot}/src/core/qtutils_core_settings.cpp"
HEADERS += $$files($${qtutilsRepoRoot}/src/tests/some_qt_tests/*.hpp,true)
HEADERS += "$${qtutilsRepoRoot}/include/qtutils/core/settings.hpp"


RC_ICONS = "$${cpputilsRepoRoot}/src/resources/cpputils/cpputils_icon.ico"

RESOURCES	+= \
	"$${qtutilsRepoRoot}/src/resources/qtutils_rcs.qrc"
	
OTHER_FILES += "$${qtutilsRepoRoot}/scripts/unix_compile_some_qt_tests_for_wasm.sh"
OTHER_FILES += "$${qtutilsRepoRoot}/scripts/.raw/_unix_compile_qt_as_wasm.sh"
