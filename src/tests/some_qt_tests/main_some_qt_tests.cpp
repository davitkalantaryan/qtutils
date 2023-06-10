//
// file:		main_focust_product01_admin_gui.cpp
// path:		src/cppc/core/focust_product01_admin_gui/main_focust_product01_admin_gui.cpp
// created on:	2021 Dec 24
// creatd by:	Davit Kalantaryan (davit.kalantaryan@desy.de)
//


#include "some_qt_tests_mainwindow.hpp"
#include <qtutils/disable_utils_warnings.h>
#include <QApplication>


int main(int a_argc, char* a_argv[])
{
	QApplication aApp(a_argc, a_argv);
	::qtutils::some_qt_tests::MainWindow aMainWindow;
	
	aMainWindow.show();
	
	QApplication::exec();
	
	return 0;
}
