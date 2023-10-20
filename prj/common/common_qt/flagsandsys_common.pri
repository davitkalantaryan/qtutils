#
# repo:			qtutils
# name:			flagsandsys_common.pri
# path:			prj/common/common_qt/flagsandsys_common.pri
# created on:   2023 Jun 21
# created by:   Davit Kalantaryan (davit.kalantaryan@desy.de)
# usage:		Use this qt include file to calculate some platform specific stuff
#


message("!!! $${PWD}/flagsandsys_common.pri")

isEmpty(qtutilsFlagsAndSysCommonIncluded){
    qtutilsFlagsAndSysCommonIncluded = 1

    qtutilsRepoRoot = $${PWD}/../../..

    isEmpty(artifactRoot) {
        artifactRoot = $$(artifactRoot)
	        isEmpty(artifactRoot) {
		        artifactRoot = $${qtutilsRepoRoot}
		}
    }

    include("$${qtutilsRepoRoot}/contrib/emsutils/prj/common/common_qt/flagsandsys_common.pri")

    INCLUDEPATH += $${qtutilsRepoRoot}/include

    LIBS	+= -L$${qtutilsRepoRoot}/sys/$${CODENAME}/$$CONFIGURATION/lib
    LIBS	+= -L$${qtutilsRepoRoot}/sys/$${CODENAME}/$$CONFIGURATION/tlib

    OTHER_FILES += $$files($${PWD}/../common_mkfl/*.Makefile,true)
    win32 {
            isEmpty( DO_NOT_DISABLE_QT_SPECIFIC_VARNINGS_V ){
	            QMAKE_CXXFLAGS += /FI"qtutils/disable_utils_warnings.h"
	    }
    }
}
