//
// repo:            qtutils
// file:            qtutils_ui_applicationwithlogin.cpp
// path:			src/ui/qtutils_ui_applicationwithlogin.cpp
// created on:		2023 Jul 02
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/ui/applicationwithloginqu.hpp>


namespace qtutils { namespace ui {


ApplicationWithLogin::~ApplicationWithLogin()
{
}


ApplicationWithLogin::ApplicationWithLogin(int& a_argc, char** a_argv)
    :
      QApplication(a_argc,a_argv)
{
}


}}  //  namespace qtutils { namespace ui {
