//
// file:            closablewindow.impl.hpp
// path:			include/qtutils/ui/closablewindow.impl.hpp
// created on:		2022 Jan 18
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#pragma once

#ifndef QTUTILS_INCLUDE_CLOSABLEWINDOW_IMPL_HPP
#define QTUTILS_INCLUDE_CLOSABLEWINDOW_IMPL_HPP

#ifndef QTUTILS_INCLUDE_CLOSABLEWINDOW_HPP
#include "closablewindow.hpp"
#endif
#include <qtutils/export_symbols.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolBar>
#include <QAction>


namespace qtutils { namespace ui{


template <typename WidgetType>
template<typename... Targs>
ClosableWindow<WidgetType>::ClosableWindow(Targs... a_args)
    :
      m_wdg(a_args...),
      m_actionClose(QIcon(":/qtutils/close_button_icon01.png"),"&Close")
{
    QWidget* pParent = m_wdg.parentWidget();
    setParent(pParent);
    m_wdg.setParent(nullptr);
    
    m_toolBar.addAction(&m_actionClose);
    m_menuAndToolbarLayout.addWidget(&m_toolBar);
    m_bottomLayout.addWidget(&m_wdg);

    m_overallLayout.addLayout(&m_menuAndToolbarLayout);
    m_overallLayout.addLayout(&m_bottomLayout);

    setLayout(&m_overallLayout);

    connect(&m_actionClose,&QAction::triggered,this,[this](){
        hide();
    });
}


template <typename WidgetType>
ClosableWindow<WidgetType>::~ClosableWindow()
{
}


template <typename WidgetType>
const WidgetType* ClosableWindow<WidgetType>::qChildWidget()const
{
    return &(m_wdg);
}


template <typename WidgetType>
WidgetType* ClosableWindow<WidgetType>::qChildWidget()
{
    return &(m_wdg);
}


template <typename WidgetType>
void ClosableWindow<WidgetType>::AddAction(QAction* a_pAction)
{
    m_toolBar.addAction(a_pAction);
}


}} // namespace qtutils { namespace ui{


#endif  // #ifndef QTUTILS_INCLUDE_CLOSABLEWINDOW_IMPL_HPP
