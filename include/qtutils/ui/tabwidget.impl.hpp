//
// file:            tabwidget.hpp
// path:			include/qtutils/ui/tabwidget.hpp
// created on:		2022 Aug 16
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_TABWIDGET_IMPL_HPP
#define QTUTILS_INCLUDE_QTUTILS_UI_TABWIDGET_IMPL_HPP

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_TABWIDGET_HPP
#include "tabwidget.hpp"
#endif


namespace qtutils { namespace ui{


template<typename... Targs>
Tab::Tab(Targs... a_args)
    :
      QLabel(a_args...)
{
    static_cast<void>(m_reserved01);
    m_isSelected = 0;
    m_index = -1;
}


template<typename... Targs>
Tab* TabBar::addTab(QWidget* a_page, Targs... a_args)
{
    return addTabRaw(a_page,new Tab(a_args...));
}


template<typename... Targs>
TabScene::TabScene(Targs... a_args)
    :
      QWidget(a_args...),
      m_pTabActiveWidget(nullptr)
{
}


}}  //  namespace qtutils { namespace ui{


#endif  //  #ifndef QTUTILS_INCLUDE_QTUTILS_UI_TABWIDGET_IMPL_HPP
