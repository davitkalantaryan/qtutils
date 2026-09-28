#
# repo:         qtutils
# name:         use_qtkeychain_cmn_no_source.pri
# path:         prj/common/common_qt/use_qtkeychain_cmn_no_source.pri
# created on:   2026 Sep 13
# created by:   Davit Kalantaryan (davit.kalantaryan@desy.de)
#


message("!!! $${PWD}/use_qtkeychain_cmn_no_source.pri")

isEmpty(qtutilsUseQtKeychainNoSourceIncluded){
    qtutilsUseQtKeychainNoSourceIncluded = 1
    include ( "$${PWD}/flagsandsys_common.pri" )
    INCLUDEPATH += $$(CPPUTILS_QTKEYCHAIN_ROOT)/include
    LIBS += $$(CPPUTILS_QTKEYCHAIN_ROOT)/lib/$${LIB_PREFIX}qt6keychain$${nameExtensionQtUtils}.$${libNameEndingQtUtils}
    HEADERS += "$${qtutilsRepoRoot}/include/qtutils/core/qtkeychain.hpp"
}
