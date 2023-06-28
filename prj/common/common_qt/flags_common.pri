#
# repo:                 qtutils
# name:			flags_common.pri
# path:			${repositoryRoot}/prj/common/common_qt/flags_common.pri
# created on:		2022 Oct 19
# created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
# usage:		Use this qt include file to calculate some platform specific stuff
#


message("!!! $${PWD}/flags_common.pri")
qtutilsFlagsCommonIncluded = 1

include ( "$${PWD}/resolve_common.pri" )
isEmpty( cpputilsFlagsCommonIncluded ) {
        include ( "$${cpputilsRepoRoot}/prj/common/common_qt/flags_common.pri" )
        cpputilsFlagsCommonIncluded = 1
}

INCLUDEPATH += $${qtutilsRepoRoot}/include
win32 {
        isEmpty( DO_NOT_DISABLE_QT_SPECIFIC_VARNINGS_V ){
	        QMAKE_CXXFLAGS += /FI"qtutils/disable_utils_warnings.h"
	}
}
