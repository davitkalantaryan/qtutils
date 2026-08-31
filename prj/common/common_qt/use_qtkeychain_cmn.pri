#
# repo:		qtutils
# name:		use_qtkeychain_cmn.pri
# path:		prj/common/common_qt/use_qtkeychain_cmn.pri
# created on:   2026 Aug 31
# created by:   Davit Kalantaryan (davit.kalantaryan@desy.de)
#


message("!!! $${PWD}/use_qtkeychain_cmn.pri")

isEmpty(qtutilsUseQtKeychainIncluded){
    qtutilsUseQtKeychainIncluded = 1
    include ( "$${PWD}/flagsandsys_common.pri" )
    INCLUDEPATH += $$(CPPUTILS_QTKEYCHAIN_ROOT)/include
    LIBS += $$(CPPUTILS_QTKEYCHAIN_ROOT)/lib/$${LIB_PREFIX}qt6keychain$${nameExtensionQtUtils}.$${libNameEndingQtUtils}
    SOURCES += "$${qtutilsRepoRoot}/src/core/qtutils_core_qtkeychain.cpp"
    HEADERS += "$${qtutilsRepoRoot}/include/qtutils/core/qtkeychain.hpp"
}
