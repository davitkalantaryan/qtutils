#
# name:			flags_common.pri
# path:			${repositoryRoot}/prj/common/common_qt/flags_common.pri
# created on:		2022 Oct 19
# created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
# usage:		Use this qt include file to calculate some platform specific stuff
#


message("!!! $${PWD}/flags_common.pri")

isEmpty( qtutilsRepoRoot ) {
        qtutilsRepoRoot = $${PWD}/../../..
}


isEmpty( NOT_USE_CPPUTILS_FLAGS_V ){
        isEmpty( cpputilsRepoRoot ){
	        cpputilsRepoRoot=$${qtutilsRepoRoot}/contrib/cpputils
	}
	include("$${cpputilsRepoRoot}/prj/common/common_qt/flags_common.pri")
}

INCLUDEPATH += $${qtutilsRepoRoot}/include

win32 {
        isEmpty( DO_NOT_DISABLE_QT_SPECIFIC_VARNINGS_V ){
	        QMAKE_CXXFLAGS += /FI"qtutils/disable_utils_warnings.h"
	}
}
