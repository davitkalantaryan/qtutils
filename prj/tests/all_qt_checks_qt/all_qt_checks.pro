#
# file:		focust_analyze.pro
# path:		prj/ui/focust_analyze_qt/focust_analyze.pro
# created on:	2022 Apr 16
# creatd by:	Davit Kalantaryan (davit.kalantaryan@desy.de)
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

CORE_SRC_DIR = $${qtutilsRepoRoot}/src/core
UI_SRC_DIR = $${qtutilsRepoRoot}/src/ui
TOOLS_SRC_DIR = $${qtutilsRepoRoot}/src/tools

SOURCES +=  $$files($$CORE_SRC_DIR/*.cpp,true)
SOURCES +=  $$files($$UI_SRC_DIR/*.cpp,true)
SOURCES +=  $$files($$TOOLS_SRC_DIR/*.cpp,true)
SOURCES +=  $$files($${cinternalRepoRoot}/src/core/*.c,true)
HEADERS +=  $$files($$qtutilsRepoRoot/include/*.hpp,true)
HEADERS +=  $$files($$qtutilsRepoRoot/include/*.h,true)

SOURCES +=	\
	$$qtutilsRepoRoot/src/tests/main_all_qt_checks.cpp

RESOURCES	+= \
	"$${qtutilsRepoRoot}/src/resources/qtutils_rcs.qrc"
