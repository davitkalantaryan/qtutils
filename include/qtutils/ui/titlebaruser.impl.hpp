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

#define QTUTILS_TITLEBARUSER_DEF_MARGIN_SIZE    2

template <typename WidgetType>
template<typename... Targs>
TitlebarUser<WidgetType>::TitlebarUser(Targs... a_args)
    :
      SizeApplyWindow<WidgetType>(a_args...),
      m_pTitleBar(new TitleBar(this)),
      m_pBelowWidget(new QWidget(this))
{
    SetMargins(QTUTILS_TITLEBARUSER_DEF_MARGIN_SIZE,QTUTILS_TITLEBARUSER_DEF_MARGIN_SIZE,QTUTILS_TITLEBARUSER_DEF_MARGIN_SIZE,QTUTILS_TITLEBARUSER_DEF_MARGIN_SIZE);
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
void TitlebarUser<WidgetType>::SetMargins(int a_leftMargin, int a_rightMargin, int a_topMargin, int a_downMargin)
{
    m_leftMargin = a_leftMargin;
    m_rightMargin = a_rightMargin;
    m_topMargin = a_topMargin;
    m_downMargin = a_downMargin;
    WidgetType::setMinimumSize(m_leftMargin+m_rightMargin+1,m_topMargin+m_downMargin+1);
}


template <typename WidgetType>
void TitlebarUser<WidgetType>::SetLeftMargin(int a_leftMargin)
{
    m_leftMargin = a_leftMargin;
    WidgetType::setMinimumSize(m_leftMargin+m_rightMargin+1,m_topMargin+m_downMargin+1);
}


template <typename WidgetType>
void TitlebarUser<WidgetType>::SetRightMargin(int a_rightMargin)
{
    m_rightMargin = a_rightMargin;
    WidgetType::setMinimumSize(m_leftMargin+m_rightMargin+1,m_topMargin+m_downMargin+1);
}


template <typename WidgetType>
void TitlebarUser<WidgetType>::SetTopMargin(int a_topMargin)
{
    m_topMargin = a_topMargin;
    WidgetType::setMinimumSize(m_leftMargin+m_rightMargin+1,m_topMargin+m_downMargin+1);
}


template <typename WidgetType>
void TitlebarUser<WidgetType>::SetDownMargin(int a_downMargin)
{
    m_downMargin = a_downMargin;
    WidgetType::setMinimumSize(m_leftMargin+m_rightMargin+1,m_topMargin+m_downMargin+1);
}


template <typename WidgetType>
int  TitlebarUser<WidgetType>::leftMargin() const
{
    return m_leftMargin;
}


template <typename WidgetType>
int  TitlebarUser<WidgetType>::rightMargin() const
{
    return m_rightMargin;
}


template <typename WidgetType>
int  TitlebarUser<WidgetType>::topMargin() const
{
    return m_topMargin;
}


template <typename WidgetType>
int  TitlebarUser<WidgetType>::downMargin() const
{
    return m_downMargin;
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
    // int            m_leftMargin, m_rightMargin, m_topMargin, m_downMargin;
    const int cnHmargins(m_leftMargin+m_rightMargin);
    m_pTitleBar->move(m_leftMargin,m_topMargin);
    m_pTitleBar->resize(cnWidthThis-cnHmargins,cnTitlebarHeight);
    m_pBelowWidget->move(m_leftMargin,cnTitlebarHeight+m_topMargin);
    m_pBelowWidget->resize(cnWidthThis-cnHmargins,a_newSize.height()-cnTitlebarHeight-m_topMargin-m_downMargin);
}


}} // namespace qtutils { namespace ui{


#endif  //  #ifndef QTUTILS_INCLUDE_QTUTILS_UI_TITLEBARUSER_IMPL_HPP
