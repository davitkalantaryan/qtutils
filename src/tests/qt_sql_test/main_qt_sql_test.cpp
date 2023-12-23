//
// file:		main_qt_sql_test.cpp
// path:		src/tests/qt_sql_test/main_qt_sql_test.cpp
// created on:	2023 Nov 21
// creatd by:	Davit Kalantaryan (davit.kalantaryan@desy.de)
//


#include <qtutils/core/sqldbwrp.hpp>
#include <stdio.h>
#include <qtutils/disable_utils_warnings.h>
#include <QCoreApplication>


int main(int a_argc, char* a_argv[])
{
	QCoreApplication aApp(a_argc, a_argv);
    
    QCoreApplication::setOrganizationDomain("cpputils.ai");
    QCoreApplication::setOrganizationName("cpputils");  
    QCoreApplication::setApplicationName("qt_sql_test");
    QCoreApplication::setApplicationVersion("1.0.0");
    
    ::qtutils::core::SqlDbWrp aDb;
    if(!aDb.InitializePostgreSQL("focust","localhost","focust","Test1234!Focust")){
        fprintf(stderr,"unable open db\n");
        return 1;
    }
    
    aDb.CleanupDb();
	
	//aApp.exec();
	
	return 0;
}
