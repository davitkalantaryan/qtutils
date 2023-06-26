#
# name:			sys_common.pri
# path:			${repositoryRoot}/prj/common/common_qt/sys_common.pri
# created on:		2022 Jan 16
# created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)  
# usage:		Use this qt include file to calculate some platform specific stuff
#


message("!!! $${PWD}/sys_common.pri")

include ( "$${PWD}/resolve_common.pri" )
include ( "$${cpputilsRepoRoot}/prj/common/common_qt/sys_common.pri" )
!isEmpty(QTUTILS_MODIFIED_APP_NAME) : TARGET = $${QTUTILS_MODIFIED_APP_NAME}
