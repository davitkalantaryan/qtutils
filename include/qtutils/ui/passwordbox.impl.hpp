//
// file:            passwordbox.impl.hpp
// path:			include/qtutils/ui/passwordbox.impl.hpp
// created on:		2022 Jan 25
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_PASSWORDBOX_IMPL_HPP
#define QTUTILS_INCLUDE_QTUTILS_UI_PASSWORDBOX_IMPL_HPP

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_PASSWORDBOX_HPP
#include "passwordbox.hpp"
#endif

namespace qtutils { namespace ui{


template<typename... Targs>
PasswordBox::PasswordBox(Targs... a_args)
    :
      WgtWithWgtInRight<QLineEdit,QLabel>(a_args...)
{
    Init();
}


}} // namespace qtutils { namespace ui{


#endif
