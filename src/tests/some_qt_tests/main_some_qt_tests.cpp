//
// file:		main_some_qt_tests.cpp
// path:		src/tests/some_qt_tests/main_some_qt_tests.cpp
// created on:	2023 Jun 10
// creatd by:	Davit Kalantaryan (davit.kalantaryan@desy.de)
//


#include "some_qt_tests_mainwindow.hpp"
#include <qtutils/core/settings.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QApplication>


int main(int a_argc, char* a_argv[])
{
	QApplication aApp(a_argc, a_argv);
    
    QCoreApplication::setOrganizationDomain("cpputils.ai");
    QCoreApplication::setOrganizationName("cpputils");  
    QCoreApplication::setApplicationName("some_qt_tests");
    QCoreApplication::setApplicationVersion(SOME_QT_TEST_VERSION);
    //QApplication::setWindowIcon( QIcon(":/qtutils/focust_logo.png") );
    ::qtutils::Settings::setDefaultFormat(QSettings::IniFormat);
    
    ::qtutils::Settings aSettings;
    
    const QString initString = aSettings.value(SOME_QT_TEST_KEY01,"No settings").toString();
    
	::qtutils::some_qt_tests::MainWindow aMainWindow(initString);
	
	aMainWindow.show();
	
	QApplication::exec();
	
	return 0;
}
