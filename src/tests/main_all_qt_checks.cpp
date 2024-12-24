//
// file:			cpputils_bigint.cpp
// path:			src/core/cpputils_hashtbl.cpp
// created on:		2021 Mar 22
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/core/logger.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QApplication>

int main(int a_argc, char* a_argv[])
{
	QApplication aApp(a_argc,a_argv);

    QtUtilsDebugLogLvlCat(1,"test")<<"Hi";
    QtUtilsDebugLogLvl(1)<<"Hi";
	
	aApp.exec();
	
	return 0;
}
