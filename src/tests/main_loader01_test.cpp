//
// file:            main_loader01_test.cpp
// path:			src/tests/main_loader01_test.cpp
// created on:		2022 Apr 01
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/ui/spinnerbox.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QApplication>


int main(int a_argc, char* a_argv[])
{
	QApplication aApp(a_argc,a_argv);
	
	qtutils::ui::SpinnerBox aSpinBox;
	
	aSpinBox.show();
	aSpinBox.StartSpinning();
	
	aApp.exec();
	
	aSpinBox.StopSpinning();
	
	return 0;
}
