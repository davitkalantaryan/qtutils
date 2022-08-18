//
// file:            passwordbox.impl.hpp
// path:			include/qtutils/ui/passwordbox.impl.hpp
// created on:		2022 Jan 25
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_TITLEBARUSER_IMPL_HPP
#define QTUTILS_INCLUDE_QTUTILS_UI_TITLEBARUSER_IMPL_HPP

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_TITLEBARUSER_HPP
#include "titlebaruser.hpp"
#endif
#include <qtutils/disable_utils_warnings.h>
#include <QResizeEvent>

namespace qtutils { namespace ui{

template <typename WidgetType>
template<typename... Targs>
TitlebarUser<WidgetType>::TitlebarUser(Targs... a_args)
    :
      SizeApplyWindow<WidgetType>(a_args...),
      m_pTitleBar(new TitleBar(this)),
      m_pBelowWidget(new QWidget(this))
{
    WidgetType::setWindowFlags(WidgetType::windowFlags()|Qt::FramelessWindowHint);
    m_pTitleBar->setParent(this);
}


template <typename WidgetType>
TitlebarUser<WidgetType>::~TitlebarUser()
{
    delete m_pBelowWidget;
    delete m_pTitleBar;
}


template <typename WidgetType>
void TitlebarUser<WidgetType>::SetTitlebar(TitleBar* a_pTitlebar)
{
    if(a_pTitlebar && (a_pTitlebar!=m_pTitleBar)){
        delete m_pTitleBar;
        m_pTitleBar = a_pTitlebar;
        a_pTitlebar->setParent(this);
        ApplyNewSize(WidgetType::size());
    }
}


template <typename WidgetType>
void TitlebarUser<WidgetType>::ReleaseOwnershipOfTitlebar()
{
    m_pTitleBar = new TitleBar(this);
    m_pTitleBar->setParent(this);
    ApplyNewSize(WidgetType::size());
}


template <typename WidgetType>
TitleBar* TitlebarUser<WidgetType>::titlebar()const
{
    return m_pTitleBar;
}


template <typename WidgetType>
void TitlebarUser<WidgetType>::SetBelowWidget(QWidget* a_pBelowWidget)
{
    if(a_pBelowWidget && (a_pBelowWidget!=m_pBelowWidget)){
        delete m_pBelowWidget;
        m_pBelowWidget = a_pBelowWidget;
        a_pBelowWidget->setParent(this);
        ApplyNewSize(WidgetType::size());
    }
}


template <typename WidgetType>
void TitlebarUser<WidgetType>::ReleaseOwnershipOfBelowWidget()
{
    m_pBelowWidget = new QWidget(this);
    ApplyNewSize(WidgetType::size());
}


template <typename WidgetType>
QWidget* TitlebarUser<WidgetType>::belowWidget()const
{
    return m_pBelowWidget;
}


template <typename WidgetType>
void TitlebarUser<WidgetType>::ApplyNewSize(const QSize& a_newSize)
{
    const int cnWidthThis(a_newSize.width());
    const int cnTitlebarHeight(m_pTitleBar->height());
    m_pTitleBar->move(0,0);
    m_pTitleBar->resize(cnWidthThis,cnTitlebarHeight);
    m_pBelowWidget->move(0,cnTitlebarHeight);
    m_pBelowWidget->resize(cnWidthThis,a_newSize.height()-cnTitlebarHeight);
}


}} // namespace qtutils { namespace ui{


#endif  //  #ifndef QTUTILS_INCLUDE_QTUTILS_UI_TITLEBARUSER_IMPL_HPP
