#
# file:			any_quick_test.pro
# path:			prj/tests/any_quick_test_qt/any_quick_test.pro
# created on:	2021 Mar 07
# created by:	Davit Kalantaryan
#

include ( "$${PWD}/../../common/common_qt/sys_common.pri" )

QT -= gui
QT -= core
QT -= widgets
CONFIG -= qt

LIBS += -pthread

repoRootPath=$${PWD}/../../..

INCLUDEPATH += "$${PWD}/../../../include"
DEFINES += QTGUTILS_USING_STATIC_LIB_OR_OBJECTS


SOURCES	+=		\
	"$${PWD}/../../../src/tests/main_any_quick_test.cpp"


HEADERS += $$files($${repoRootPath}/include/*.h,true)
HEADERS += $$files($${repoRootPath}/include/*.h,true)
