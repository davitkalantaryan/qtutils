//
// repo:            qtutils
// file:            main_networkacccessmanager_test01.cpp
// path:			src/tests/main_networkacccessmanager_test01.cpp
// created on:		2024 Jun 06
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//


#include <qtutils/core/networkaccessmanagerqu.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QCoreApplication>


int main(int a_argc, char* a_argv[])
{
    QCoreApplication aApp(a_argc, a_argv);
    qtutils::network::AccessManager aMngr;
}
