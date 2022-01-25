//
// file:            webdialog.impl.hpp
// path:			include/qtutils/ui/webdialog.impl.hpp
// created on:		2022 Jan 19
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_WEBDIALOG_IMPL_HPP
#define QTUTILS_INCLUDE_QTUTILS_UI_WEBDIALOG_IMPL_HPP


#ifndef QTUTILS_INCLUDE_WEBSTYLELOGINTYPEWINDOW_HPP
#include "webdialog.hpp"
#endif

#include <qtutils/ui/global_functions.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QApplication>
#ifdef QTUTILS_MAKE_DEBUG
#include <qtutils/core/logger.hpp>
#endif


namespace qtutils { namespace ui{


template <typename WidgetType>
template<typename... Targs>
WebDialog<WidgetType>::WebDialog(Targs... a_args)
	:
	  WidgetType(a_args...)
{
    m_con1=QObject::connect(qApp,&QApplication::focusChanged,[this](QWidget*,QWidget* a_now){    
#ifdef QTUTILS_MAKE_DEBUG
        QtUtilsDebug()<<"a_not:"<<a_now;
#endif
        if(a_now && (!isSameWidgetOrChild(this,a_now))){
            WidgetType::deleteLater();
        }
    });
    
    m_con2=QObject::connect(this,&QDialog::finished,this,[this](int a_result){
#ifdef QTUTILS_MAKE_DEBUG
        qDebug()<<a_result;
#else
        static_cast<void>(a_result);
#endif
        WidgetType::deleteLater();
    });
}


template <typename WidgetType>
WebDialog<WidgetType>::~WebDialog()
{
    QObject::disconnect(m_con2);
    QObject::disconnect(m_con1);
}


}}  // namespace qtutils { namespace ui{


#endif  // #ifndef QTUTILS_INCLUDE_QTUTILS_UI_WEBDIALOG_IMPL_HPP
