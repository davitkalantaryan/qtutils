//
// repo:            FocusTeam p01 common
// file:            focust_p01_applicationbase.cpp
// path:			src/p01/focust_p01_applicationbase.cpp
// created on:		2023 Jul 01
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <focust/p01/applicationbase.hpp>


namespace focust { namespace p01 {


ApplicationBase::~ApplicationBase()
{
}


ApplicationBase::ApplicationBase(int& a_argc, char** a_argv)
    :
      QApplication(a_argc,a_argv)
{
}


}}  //  namespace focust { namespace p01 {
