#
# file:		loader01_test.pro
# path:		prj/tests/loader01_test_qt/loader01_test.pro
# created on:	2022 Apr 01
# creatd by:	Davit Kalantaryan (davit.kalantaryan@desy.de)
#

message("!!!!!!!!!!! loader01_test.pro")

!isEmpty(MONITOR_TEST_TARGET_NAME) : TARGET = $${MONITOR_TEST_TARGET_NAME}


# QT += svg
# QT += charts

include ( "$${PWD}/../../common/common_qt/sys_common.pri" )
include ( "$${PWD}/../../common/common_qt/gui_common.pri" )
include ( "$${PWD}/../../common/common_qt/qtutils_common.pri" )

DESTDIR     = $${artifactRoot}/$${SYSTEM_PATH}/tests

INCLUDEPATH += $${cpputilsRepoRoot}/include

SOURCES += "$${qtutilsRepoRoot}/src/ui/qtutils_ui_passwordbox.cpp"

SOURCES +=	\
	"$${repositoryRoot}/src/tests/main_loader01_test.cpp"		\
	"$${repositoryRoot}/src/ui/qtutils_ui_spinnerbox.cpp"		\
	"$${repositoryRoot}/src/ui/qtutils_ui_global_functions.cpp"
	
HEADERS +=	\
	"$${repositoryRoot}/include/qtutils/ui/spinnerbox.hpp"		\
	"$${repositoryRoot}/include/qtutils/ui/spinnerbox.impl.hpp"	\
	"$${repositoryRoot}/include/qtutils/ui/spinnerparent.hpp"	\
	"$${repositoryRoot}/include/qtutils/ui/spinnerparent.impl.hpp"	\
	"$${repositoryRoot}/include/qtutils/ui/global_functions.hpp"


#RC_ICONS = $${repositoryRoot}/src/resources/img/qtutils_logo.ico

RESOURCES	+= \
	"$${qtutilsRepoRoot}/src/resources/qtutils_rcs.qrc"
