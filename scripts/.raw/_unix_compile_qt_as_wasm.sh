#!/bin/bash
#
# example:  _unix_compile_qt_as_wasm.sh password_hasher2 prj/utils/password_hasher_qt src/resources/img/focust_logo.svg src/resources/img/focust_logo.png
#
# set following environment variables
# 1. QT_ROOT_DIR   example => $ export QT_ROOT_DIR=~/qt_compiled/5.15.2
#                               export QT_ROOT_DIR=~/Qt/5.15.2
# 2. DS_PEM_FILE  example => $ export DS_PEM_FILE=~/.ssh/aws_key2.pem # no need for this
# 3. DS_USERNAME (username to access aws or other http servers, 
#                if not specifed, then local username will be used  
#                       [ex. $ export DS_USERNAME=ubuntu ])
#

currentDirectory=`pwd`

# https://intoli.com/blog/exit-on-errors-in-bash-scripts/
# exit when any command fails
set -e

# keep track of the last executed command
trap 'last_command=$current_command; current_command=$BASH_COMMAND' DEBUG
# echo an error message before exiting
#trap 'echo "\"${last_command}\" command finished with exit code $?."' EXIT

if [ $# -lt 2 ]; then
        echo "at least target name and qt pro file should be specified"
	exit 1
fi
targetName=$1
qtProjectileDir=$2
svgFilePath=$3
pngFilePath=$4
svgFileName=`basename "${svgFilePath}"`


# in mac short directory calculation based on n'readlink' or 'realpath' will not work
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
cd ../..
# now we are in the repository root
if [ -z "$repositoryRoot" ]; then
	repositoryRoot=`pwd`
fi
source ./scripts/unix_per_session.sh ./scripts/unix_per_session.sh 1

if [ -z "$QT_ROOT_DIR" ]; then
	echo "QT_ROOT_DIR environment variable is not set"
	exit 1
fi
if [ ! -d $QT_ROOT_DIR ]; then
	echo "QT_ROOT_DIR environment variable does not point to directory"
	exit 1
fi
if [ ! -d $QT_ROOT_DIR/wasm_32 ]; then
	echo "No WASM toolchain in the qt"
	exit 1
fi


cd ${currentDirectory}
cd "$qtProjectileDir"
"$QT_ROOT_DIR/wasm_32/bin/qmake" CONFIG+=release CONFIG-=debug QTUTILS_MODIFIED_APP_NAME=${targetName}


make -f Makefile.${targetName}.wasm.Release
# there is no necessity of below line, because of set -e, but who knows :)
if [ $? -ne 0 ]; then
   echo "Unable to compile"
   exit 1
fi


dateVar=`date`

# we can do rebranding (see https://www.qt.io/blog/2018/11/19/getting-started-qt-webassembly)
cd "${repositoryRoot}/sys/wasm/Release/bin/${targetName}"
sed -i "s/Qt for WebAssembly/ prepared by Qt webassembly compiled on ${dateVar}/g" ${targetName}.html
sed -i 's/qtlogo.svg/${svgFileName}/g' ${targetName}.html
sed -i 's/<head>/<head>\n    <link rel="icon" href="focust_logo.png">/' ${targetName}.html
rm qtlogo.svg
cd ${currentDirectory}
cp "$svgFilePath" ${repositoryRoot}/sys/wasm/Release/bin/${targetName}/.
cp "$pngFilePath" ${repositoryRoot}/sys/wasm/Release/bin/${targetName}/.
