#
# file:		password_hasher.pro
# path:		prj/utils/password_hasher_qt/password_hasher.pro
# created on:	2022 Feb 16
# creatd by:	Davit Kalantaryan (davit.kalantaryan@desy.de)
#

#DEFINES += QTUTILS_UI_FORCE_WEB_STYLE
#DEFINES += QTUTILS_CORE_FORCE_NEW_SETTINGS
#DEFINES += QTUTILS_MAKE_DEBUG

DEFINES += CPPUTILS_HASH_NO_NEW_DELETE
DEFINES += FOCUST_MAKE_EVERYTHING_PRIVATE


message("!!!!!!!!!!! focust_product01_admin_gui.pro")

# QT += svg
# QT += charts

include ( "$${PWD}/../../common/common_qt/sys_common.pri" )
include ( "$${qtutilsRepoRoot}/prj/common/common_qt/flags_common.pri" )


QT += widgets
QT += network
#QT += sql
QT += websockets

CONFIG += c++17
#QTPLUGIN += qtvirtualkeyboardplugin


SOURCES += $$files($${qtutilsRepoRoot}/src/tests/some_qt_tests/*.cpp,true)
HEADERS += $$files($${qtutilsRepoRoot}/src/tests/some_qt_tests/*.hpp,true)
#HEADERS += $$files($${qtutilsRepoRoot}/include/*.hpp,true)
#HEADERS += $$files($${qtutilsRepoRoot}/include/*.h,true)



RC_ICONS = "$${cpputilsRepoRoot}/src/resources/cpputils/cpputils_icon.ico"

RESOURCES	+= \
	"$${qtutilsRepoRoot}/src/resources/qtutils_rcs.qrc"
	
OTHER_FILES += "$${qtutilsRepoRoot}/scripts/unix_compile_some_qt_tests_for_wasm.sh"
OTHER_FILES += "$${qtutilsRepoRoot}/scripts/.raw/_unix_compile_qt_as_wasm.sh"
