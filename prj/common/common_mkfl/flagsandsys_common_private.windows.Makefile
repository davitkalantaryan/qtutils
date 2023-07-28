#
# repo:		cpputils
# file:		flagsandsys_common_private.windows.Makefile
# created on:	2020 Dec 14
# created by:	Davit Kalantaryan (davit.kalantaryan@desy.de)
# purpose:	This file can be only as include
#

!IFNDEF cpputilsRepoRoot
cpputilsRepoRoot	= $(MAKEDIR)\..\..\..
!ENDIF
!include <$(cpputilsRepoRoot)\prj\common\common_mkfl\flagsandsys_common_private_pure.windows.Makefile>


!IFNDEF cinternalRepoRoot
cinternalRepoRoot	= $(cpputilsRepoRoot)\contrib\cinternal
!ENDIF
!include <$(cinternalRepoRoot)\prj\common\common_mkfl\flagsandsys_common_pure.windows.Makefile>
