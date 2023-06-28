#
# name:			sys_common.pri
# path:			${repositoryRoot}/prj/common/common_qt/sys_common.pri
# created on:		2022 Jan 16
# created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)  
# usage:		Use this qt include file to calculate some platform specific stuff
#


message("!!! $${PWD}/sys_common.pri")
qtutilsSysCommonIncluded = 1

include ( "$${PWD}/resolve_common.pri" )
isEmpty( cpputilsSysCommonIncluded ) {
        include ( "$${cpputilsRepoRoot}/prj/common/common_qt/sys_common.pri" )
        cpputilsSysCommonIncluded = 1
}

!isEmpty(QTUTILS_MODIFIED_APP_NAME) : TARGET = $${QTUTILS_MODIFIED_APP_NAME}
