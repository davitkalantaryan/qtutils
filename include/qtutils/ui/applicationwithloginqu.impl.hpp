//
// repo:            qtutils
// file:            applicationwithlogin.hpp
// path:			include/ui/applicationwithlogin.hpp
// created on:		2023 Jul 01
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_APPLICATIONWITHLOGIN_IMPL_HPP
#define QTUTILS_INCLUDE_QTUTILS_UI_APPLICATIONWITHLOGIN_IMPL_HPP

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_APPLICATIONWITHLOGIN_HPP
#include <qtutils/ui/applicationwithloginqu.hpp>
#endif


namespace qtutils { namespace ui {


template <typename TypeApp>
bool ApplicationWithLoginBase<TypeApp>::isVisible()const
{
    return true;
}


template <typename TypeApp>
void ApplicationWithLoginBase<TypeApp>::SetVisible()
{
}


template <typename TypeApp>
void ApplicationWithLoginBase<TypeApp>::SetInvisible()
{
}


template <typename TypeApp>
bool ApplicationWithLoginBase<TypeApp>::hasInvisiblePossibility()const
{
    return false;
}


}}  //  namespace qtutils { namespace ui {


#endif  //  #ifndef QTUTILS_INCLUDE_QTUTILS_UI_APPLICATIONWITHLOGIN_IMPL_HPP
