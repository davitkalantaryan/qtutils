#
# file:			googletest_getter.pro
# path:			prj/tests/googletest_mult/googletest_getter.pro
# created on:		2021 Mar 27
# created by:		Davit Kalantaryan
#

include ( "$${PWD}/../../common/common_qt/sys_common.pri" )

# we reevaluate this
DESTDIR = "$${PRJ_PWD}/$${SYSTEM_PATH}/test"

QT -= gui
QT -= core
QT -= widgets
CONFIG -= qt

win32{
} else {
	LIBS += "$${PWD}/../../../sys/$$CODENAME/$$CONFIGURATION/lib/libgtest_main$${nameExtension}.a"
	LIBS += "$${PWD}/../../../sys/$$CODENAME/$$CONFIGURATION/lib/libgtest$${nameExtension}.a"
	LIBS += -pthread
}

repoRootPath=$${PWD}/../../..
SRC_DIR=$${repoRootPath}/src/tests/googletest

INCLUDEPATH += "$${PWD}/../../../include"
INCLUDEPATH += "$${PWD}/../../../contrib/googletest/googletest/include"

SOURCES += $$files($${SRC_DIR}/*.cpp,true)
#SOURCES += $$files($${repoRootPath}/src/core/*.cpp,true)

HEADERS =  $$files($${repoRootPath}/include/*.h,true)
HEADERS += $$files($${repoRootPath}/include/*.hpp,true)
HEADERS += $$files($${SRC_DIR}/*.h,true)
HEADERS += $$files($${SRC_DIR}/*.hpp,true)

OTHER_FILES +=	\
	"$${PWD}/unix.Makefile"										\
	"$${PWD}/windows.Makefile"									\
	"$${PWD}/packages.config"
