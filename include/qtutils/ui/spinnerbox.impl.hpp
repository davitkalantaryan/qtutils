//
// file:            spinnerbox.impl.hpp
// path:			include/qtutils/ui/spinnerbox.impl.hpp
// created on:		2022 Apr 01
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_SPINNERBOX_IMPL_HPP
#define QTUTILS_INCLUDE_QTUTILS_UI_SPINNERBOX_IMPL_HPP

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_SPINNERBOX_HPP
#include "spinnerbox.hpp"
#endif

namespace qtutils { namespace ui{

template<typename... Targs>
SpinnerBox::SpinnerBox(Targs... a_args)
    :
      QLabel(a_args...),
      m_spbox_data_p(CreateSpinnerBox_p(this))
{
}


}} // namespace qtutils { namespace ui{


#endif  // #ifndef QTUTILS_INCLUDE_QTUTILS_UI_SPINNERBOX_IMPL_HPP
