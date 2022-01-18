//
// file:            closablewindow.hpp
// path:			include/qtutils/ui/closablewindow.hpp
// created on:		2022 Jan 18
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_CLOSABLEWINDOW_HPP
#define QTUTILS_INCLUDE_CLOSABLEWINDOW_HPP


#include <qtutils/qtutils_internal_header.h>
#include <qtutils/ui/resizible_window.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolBar>
#include <QAction>


namespace qtutils { namespace ui{


template <typename WidgetType >
class ClosableWindow : public ::qtutils::ui::ResizibleWindow<QWidget>
{
public:
    template<typename... Targs>
    ClosableWindow(const QString& a_settingsKey, Targs... a_args);
    virtual ~ClosableWindow();

    WidgetType* qChildWidget()const;
    void AddAction(QAction* a_pAction);
	
private:
    QVBoxLayout				m_overallLayout;
    QVBoxLayout				m_menuAndToolbarLayout;
    QHBoxLayout             m_bottomLayout;
    QToolBar                m_toolBar;
    WidgetType              m_wdg;
    QAction					m_actionClose;
    
};

}}  // namespace qtutils { namespace ui{


#ifndef QTUTILS_INCLUDE_CLOSABLEWINDOW_IMPL_HPP
#include "closablewindow.impl.hpp"
#endif


#endif  // #ifndef QTUTILS_INCLUDE_CLOSABLEWINDOW_HPP
