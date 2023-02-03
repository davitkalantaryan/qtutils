

mkfile_path		=  $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir		=  $(shell dirname $(mkfile_path))
QtutilsRepositoryRoot	:= $(shell curDir=`pwd` && cd $(mkfile_dir)/../../.. && pwd && cd ${curDir})
cpputilsRepoRoot	=  $(QtutilsRepositoryRoot)/contrib/cpputils

include $(cpputilsRepoRoot)/prj/common/common_mkfl/unix.common.Makefile
