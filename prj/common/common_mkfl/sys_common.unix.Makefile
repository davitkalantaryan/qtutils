

mkfile_path			=  $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir			=  $(shell dirname $(mkfile_path))

ifndef qtutilsRepoRoot
        qtutilsRepoRoot	:= $(shell curDir=`pwd` && cd $(mkfile_dir)/../../.. && pwd && cd ${curDir})
endif

ifndef repositoryRoot
        repositoryRoot	= $(qtutilsRepoRoot)
endif

ifndef artifactRoot
        artifactRoot	= $(repositoryRoot)
endif

ifndef cpputilsRepoRoot
        cpputilsRepoRoot	= $(qtutilsRepoRoot)/contrib/cpputils
endif


COMMON_FLAGS	+= -I$(qtutilsRepoRoot)/include
include $(cpputilsRepoRoot)/prj/common/common_mkfl/sys_common.unix.Makefile
