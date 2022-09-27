//
// file:            searchbox.impl.hpp
// path:			include/qtutils/ui/searchbox.impl.hpp
// created on:		2022 Sep 26
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_SEARCHBOX_IMPL_HPP
#define QTUTILS_INCLUDE_QTUTILS_UI_SEARCHBOX_IMPL_HPP

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_SEARCHBOX_HPP
#include "searchbox.hpp"
#endif

namespace qtutils { namespace ui{


template<typename... Targs>
SearchBox::SearchBox(Targs... a_args)
    :
      WgtWithWgtInLeft<QWidget,QLabel>(a_args...),
      m_sb_data_p(Init(this))
{
}


}} // namespace qtutils { namespace ui{


#endif  //  #ifndef QTUTILS_INCLUDE_QTUTILS_UI_SEARCHBOX_IMPL_HPP
