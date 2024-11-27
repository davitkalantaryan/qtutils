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


bool ApplicationWithLogin::isVisible()const
{
    return true;
}


void ApplicationWithLogin::SetVisible()
{
}


void ApplicationWithLogin::SetInvisible()
{
}


bool ApplicationWithLogin::hasInvisiblePossibility()const
{
    return false;
}


}}  //  namespace qtutils { namespace ui {
