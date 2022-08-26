//
// file:            passwordbox02.impl.hpp
// path:			include/qtutils/ui/passwordbox02.impl.hpp
// created on:		2022 Aug 20
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_PASSWORDBOX02_IMPL_HPP
#define QTUTILS_INCLUDE_QTUTILS_UI_PASSWORDBOX02_IMPL_HPP

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_PASSWORDBOX02_HPP
#include "passwordbox02.hpp"
#endif

namespace qtutils { namespace ui{

template<typename... Targs>
PasswordBox02::PasswordBox02(Targs... a_args)
    :
      WgtWithLblInRight<QLineEdit>(a_args...)
{
    InitLabel();
}

}} // namespace qtutils { namespace ui{


#endif
