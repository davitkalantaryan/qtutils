

message("!!! $${_PRO_FILE_}")
TEMPLATE = subdirs
#CONFIG += ordered

include ( "$${PWD}/../../prj/common/common_qt/flagsandsys_common_private.pri" )

QT += widgets
QT += network
QT += sql
QT += dbus


SUBDIRS		+=	"$${qtutilsRepoRoot}/prj/tests/any_quick_test_qt/any_quick_test.pro"
SUBDIRS		+=	"$${qtutilsRepoRoot}/prj/tests/qtutils_unit_test_mult/qtutils_unit_test.pro"
SUBDIRS		+=	"$${qtutilsRepoRoot}/prj/tests/all_qt_checks_qt/all_qt_checks.pro"

emsutilsFromHere{
        SUBDIRS	+= "$${emsutilsRepoRoot}/workspaces/emsutils_all_qt/emsutils_all.pro"
}

OTHER_FILES += $$files($${qtutilsRepoRoot}/scripts/*.sh,true)
OTHER_FILES += $$files($${qtutilsRepoRoot}/scripts/*.bat,true)
OTHER_FILES += $$files($${qtutilsRepoRoot}/scripts/.cicd/*.sh,true)
OTHER_FILES += $$files($${qtutilsRepoRoot}/scripts/.cicd/*.bat,true)
OTHER_FILES += $$files($${qtutilsRepoRoot}/scripts/.raw/*.sh,true)
OTHER_FILES += $$files($${qtutilsRepoRoot}/scripts/.raw/*.bat,true)
OTHER_FILES += $$files($${qtutilsRepoRoot}/docs/*.md,true)
OTHER_FILES += $$files($${qtutilsRepoRoot}/docs/*.txt,true)
OTHER_FILES += $$files($${qtutilsRepoRoot}/.github/*.yml,true)
OTHER_FILES += $$files($${qtutilsRepoRoot}/prj/common/common_mkfl/*.Makefile)


OTHER_FILES	+=	\
        "$${qtutilsRepoRoot}/.gitattributes"						\
	"$${qtutilsRepoRoot}/.gitignore"						\
	"$${qtutilsRepoRoot}/.gitmodules"						\
	"$${qtutilsRepoRoot}/LICENSE"							\
	"$${qtutilsRepoRoot}/README.md"
