#
# file:		create_jwt_test.pro
# path:		prj/tests/create_jwt_test_qt/create_jwt_test.pro
# created on:	2022 Oct 19
# creatd by:	Davit Kalantaryan (davit.kalantaryan1980@gmail.com)
#

message("!!! $${_PRO_FILE_}")

include ( "$${PWD}/../../common/common_qt/flagsandsys_common_private.pri" )

SOURCES += "$${qtutilsRepoRoot}/src/core/qtutils_core_global_functions.cpp"
SOURCES += "$${qtutilsRepoRoot}/src/core/qtutils_core_create_jwt.cpp"
SOURCES += "$${qtutilsRepoRoot}/src/tests/main_create_jwt_test.cpp"

HEADERS += "$${qtutilsRepoRoot}/include/qtutils/core/global_functions.hpp"
HEADERS += "$${qtutilsRepoRoot}/include/qtutils/core/create_jwt.hpp"
