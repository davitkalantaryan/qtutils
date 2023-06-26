#
# name:			sys_common.pri
# path:			${repositoryRoot}/prj/common/common_qt/sys_common.pri
# created on:		2022 Jan 16
# created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)  
# usage:		Use this qt include file to calculate some platform specific stuff
#


message("!!! $${PWD}/resolve_common.pri")

isEmpty( qtutilsRepoRoot ) {
        qtutilsRepoRoot = $$(qtutilsRepoRoot)
        isEmpty(qtutilsRepoRoot) {
            qtutilsRepoRoot = $${PWD}/../../..
        }
}

isEmpty( repositoryRoot ) {
        repositoryRoot = $$(repositoryRoot)
        isEmpty(repositoryRoot) {
            repositoryRoot = $${qtutilsRepoRoot}
        }
}

isEmpty(artifactRoot) {
    artifactRoot = $$(artifactRoot)
    isEmpty(artifactRoot) {
        artifactRoot = $${repositoryRoot}
    }
}


isEmpty( cpputilsRepoRoot ) {
        cpputilsRepoRoot = $$(cpputilsRepoRoot)
        isEmpty(cpputilsRepoRoot) {
            cpputilsRepoRoot=$${qtutilsRepoRoot}/contrib/cpputils
        }
}
