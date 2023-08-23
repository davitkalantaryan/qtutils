#
# file:			googletest_getter.pro
# path:			prj/tests/googletest_mult/googletest_getter.pro
# created on:		2021 Mar 27
# created by:		Davit Kalantaryan
#

include ( "$${PWD}/../../common/common_qt/sys_common.pri" )

# we reevaluate this
DESTDIR = "$${PRJ_PWD}/$${SYSTEM_PATH}/test"

QT += gui
#QT -= core
QT += widgets
#CONFIG -= qt

win32{
} else {
	LIBS += "$${cpputilsRepoRoot}/sys/$$CODENAME/$$CONFIGURATION/lib/libgtest_main$${nameExtension}.a"
	LIBS += "$${cpputilsRepoRoot}/sys/$$CODENAME/$$CONFIGURATION/lib/libgtest$${nameExtension}.a"
	LIBS += -pthread
}

SRC_DIR=$${qtutilsRepoRoot}/src/tests/googletest

INCLUDEPATH += "$${qtutilsRepoRoot}/include"
INCLUDEPATH += "$${cpputilsRepoRoot}/include"
INCLUDEPATH += "$${cpputilsRepoRoot}/contrib/googletest/googletest/include"

SOURCES += $$files($${SRC_DIR}/*.cpp,true)
#SOURCES += $$files($${repoRootPath}/src/core/*.cpp,true)

HEADERS =  $$files($${qtutilsRepoRoot}/include/*.h,true)
HEADERS += $$files($${qtutilsRepoRoot}/include/*.hpp,true)
HEADERS += $$files($${SRC_DIR}/*.h,true)
HEADERS += $$files($${SRC_DIR}/*.hpp,true)

OTHER_FILES +=	\
	"$${PWD}/unix.Makefile"										\
	"$${PWD}/windows.Makefile"									\
	"$${PWD}/packages.config"
