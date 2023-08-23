#
# repo:		qtutils
# file:		flagsandsys_common.windows.Makefile
# created on:	2020 Dec 14
# created by:	Davit Kalantaryan (davit.kalantaryan@desy.de)
# purpose:	This file can be only as include
#


!IFNDEF cpputilsRepoRoot
qtutilsRepoRoot	= $(MAKEDIR)\..\..\..
!ENDIF
!include <$(qtutilsRepoRoot)\prj\common\common_mkfl\flagsandsys_common_pure.windows.Makefile>


!IFNDEF emsutilsRepoRoot
emsutilsRepoRoot	= $(qtutilsRepoRoot)\contrib\emsutils
!ENDIF
!include <$(emsutilsRepoRoot)\prj\common\common_mkfl\flagsandsys_common_pure.windows.Makefile>
