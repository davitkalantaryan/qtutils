#
# file:		parse_jwt_test.pro
# path:		prj/tests/parse_jwt_test_qt/parse_jwt_test.pro
# created on:	2022 Oct 19
# creatd by:	Davit Kalantaryan (davit.kalantaryan1980@gmail.com)
#

message("!!! $${_PRO_FILE_}")

include ( "$${PWD}/../../common/common_qt/flagsandsys_common_private.pri" )

INCLUDEPATH += /home/kalantar/Qt/6.5.0/Src/qtwebengine/src/3rdparty/chromium/third_party/boringssl/src/include

SOURCES += "$${qtutilsRepoRoot}/src/core/qtutils_core_global_functions.cpp"
SOURCES += "$${qtutilsRepoRoot}/src/core/qtutils_core_parse_jwt.cpp"
SOURCES += "$${qtutilsRepoRoot}/src/tests/main_parse_jwt_test.cpp"

HEADERS += "$${qtutilsRepoRoot}/include/qtutils/core/global_functions.hpp"
HEADERS += "$${qtutilsRepoRoot}/include/qtutils/core/parse_jwt.hpp"
