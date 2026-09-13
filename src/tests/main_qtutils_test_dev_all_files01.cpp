//
// repo:            qtutils
// file:            main_qtutils_test_dev_all_files01.cpp
// path:			src/tests/main_qtutils_test_dev_all_files01.cpp
// created on:		2026 Sep 13
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//



#include <qtutils/core/create_jwt.hpp>
#include <qtutils/core/global_functions.hpp>
#include <qtutils/core/global_functions_02.hpp>
#include <qtutils/core/http_data01.hpp>
#include <qtutils/core/httpserver.hpp>
#include <qtutils/core/initdeinit.hpp>
#include <qtutils/core/invokeblocked.hpp>
#include <qtutils/core/logger.hpp>
#include <qtutils/core/loggertofile.hpp>
#include <qtutils/core/networkaccessmanagerv02qu.hpp>
#include <qtutils/core/parse_http_multipart.hpp>
#include <qtutils/core/parse_jwt.hpp>
#include <qtutils/core/qtkeychain.hpp>
#include <qtutils/core/sql.hpp>
#include <qtutils/core/template_functions01.hpp>
#include <qtutils/core/threadls.hpp>
#include <qtutils/core/threadorinitdeinit.hpp>
#include <qtutils/core/tokencarier.hpp>
#include <qtutils/core/utils.hpp>
#include <qtutils/core/versionnumber.hpp>

#include <qtutils/ui/closablewindow.hpp>
#include <qtutils/ui/enterevent.hpp>
#include <qtutils/ui/global_functions.hpp>
#include <qtutils/ui/logintypewindow.hpp>
#include <qtutils/ui/logwndqu.hpp>
#include <qtutils/ui/passwordbox.hpp>
#include <qtutils/ui/resiziblewindow.hpp>
#include <qtutils/ui/searchbox.hpp>
#include <qtutils/ui/sizeapplywindow.hpp>
#include <qtutils/ui/sizeresource.hpp>
#include <qtutils/ui/spinnerbox.hpp>
#include <qtutils/ui/spinnerparent.hpp>
#include <qtutils/ui/tabwidget.hpp>
#include <qtutils/ui/titlebar.hpp>
#include <qtutils/ui/titlebaruser.hpp>
#include <qtutils/ui/treeview_model.hpp>
#include <qtutils/ui/webdialog.hpp>
#include <qtutils/ui/webstylelogintypewindow.hpp>
#include <qtutils/ui/webstylelogintypewindow02.hpp>
#include <qtutils/ui/wgtwithwgtinleft.hpp>
#include <qtutils/ui/wgtwithwgtinright.hpp>

#include <qtutils/disable_utils_warnings.h>
#include <qtutils/export_symbols.h>
#include <qtutils/internal_header.h>
#include <qtutils/sizes.h>
#include <qtutils/version_resolving.h>

#include <cinternal/disable_compiler_warnings.h>
#include <qtutils/disable_utils_warnings.h>
#include <QApplication>
#include <cinternal/undisable_compiler_warnings.h>


int main(int a_argc, char* a_argv[])
{
    QApplication aApp(a_argc,a_argv);

    aApp.exec();

    return 0;
}
