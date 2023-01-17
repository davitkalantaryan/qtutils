

mkfile_path		=  $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir		=  $(shell dirname $(mkfile_path))
cpputilsRepoRoot
QtutilsRepositoryRoot	:= $(shell curDir=`pwd` && cd $(mkfile_dir)/../../.. && pwd && cd ${curDir})

include $(QtutilsRepositoryRoot)/contrib/cpputils/prj/common/common_mkfl/unix.common.Makefile
