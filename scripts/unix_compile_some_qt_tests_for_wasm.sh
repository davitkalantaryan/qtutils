#!/bin/bash
#
# example:  _unix_compile_qt_as_wasm.sh prj/utils/password_hasher_qt src/resources/qtutils/focust_logo.svg src/resources/qtutils/focust_logo.png password_hasher2
#
# set following environment variables
# 1. QT_ROOT_DIR   example => $ export QT_ROOT_DIR=~/qt_compiled/5.15.2
#                               export QT_ROOT_DIR=~/Qt/5.15.2
# 2. DS_PEM_FILE  example => $ export DS_PEM_FILE=~/.ssh/aws_key2.pem # no need for this
# 3. DS_USERNAME (username to access aws or other http servers, 
#                if not specifed, then local username will be used  
#                       [ex. $ export DS_USERNAME=ubuntu ])
#


# in mac os above short calculation will not work
# also realpath utilit is missing in mac

scriptDirectory=`dirname "${0}"`
scriptFileName=`basename "${0}"`
cd "${scriptDirectory}"
fileOrigin=`readlink "${scriptFileName}"` || :
while [ ! -z "${fileOrigin}" ]
do
	scriptDirectory=`dirname "${fileOrigin}"`
	scriptFileName=`basename "${fileOrigin}"`
	cd "${scriptDirectory}"
	fileOrigin=`readlink "${scriptFileName}"`  || :
done
cd ..
repositoryRoot=`pwd`
echo repositoryRoot=$repositoryRoot

source ./scripts/unix_per_session.sh ./scripts/unix_per_session.sh 1
cd ${repositoryRoot}

${repositoryRoot}/scripts/.raw/_unix_compile_qt_as_wasm.sh				\
	${repositoryRoot}/prj/tests/some_qt_tests_qt					\
	"$repositoryRoot/contrib/cpputils/src/resources/cpputils/cpputils_svg.svg"	\
	"$repositoryRoot/contrib/cpputils/src/resources/cpputils/cpputils_logo.png"	

