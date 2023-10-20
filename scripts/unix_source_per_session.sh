

if [ $# -eq 0 ]; then
	if [ $0 != "/bin/bash" -a $0 != "bash" -a $0 != "-bash" ]; then
		sourcePath_local=`bash -c "source $0 1"`
	else
		sourcePath_local=${BASH_SOURCE[0]}
	fi
else
	if [ $# -eq 1 ]; then
		echo ${BASH_SOURCE[0]}
		return 0
	else
		sourcePath_local=${1}
	fi
fi


makeMainJob (){

	local sourcePath=${sourcePath_local}
	unset sourcePath_local
	echo sourcePath=$sourcePath

	local currentDirectory=`pwd`
	echo currentDirectory=$currentDirectory

	# in mac short directory calculation based on n'readlink' or 'realpath' will not work
	local scriptDirectory=`dirname "${sourcePath}"`
	local scriptFileName=`basename "${sourcePath}"`
	cd "${scriptDirectory}"
	local fileOrigin=`readlink "${scriptFileName}"` || :
	while [ ! -z "${fileOrigin}" ]
	do
		local scriptDirectory=`dirname "${fileOrigin}"`
		local scriptFileName=`basename "${fileOrigin}"`
		cd "${scriptDirectory}"
		local fileOrigin=`readlink "${scriptFileName}"`  || :
	done
	cd ..
	qtutilsRepoRoot=`pwd`
	export qtutilsRepoRoot
	echo qtutilsRepoRoot=$qtutilsRepoRoot

	source ${qtutilsRepoRoot}/contrib/emsutils/scripts/unix_source_per_session.sh ${qtutilsRepoRoot}/contrib/emsutils/scripts/unix_source_per_session.sh ${qtutilsRepoRoot}/contrib/emsutils/scripts/unix_source_per_session.sh

	case "$LD_LIBRARY_PATH" in
		${qtutilsRepoRoot}/sys/$lsbCode/Debug/lib:${qtutilsRepoRoot}/sys/$lsbCode/Release/lib:* )
			echo "LD_LIBRARY_PATH for qtutils has been already set"
			;;
		* )
			export LD_LIBRARY_PATH=${qtutilsRepoRoot}/sys/$lsbCode/Debug/lib:${qtutilsRepoRoot}/sys/$lsbCode/Release/lib::$LD_LIBRARY_PATH
			;;
	esac

	cd ${currentDirectory}
}


makeMainJob
return 0
