//
// file:            resizible_window.impl.hpp
// path:			include/focust/ui/resizible_window.impl.hpp
// created on:		2021 Dec 21
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_RESIZIBLE_WINDOW_IMPL_HPP
#define QTUTILS_INCLUDE_RESIZIBLE_WINDOW_IMPL_HPP


#ifndef QTUTILS_INCLUDE_RESIZIBLE_WINDOW_HPP
#include "resizible_window.hpp"
#endif

#include <cpputils/flagshelper.h>
#include <qtutils/core/settings.hpp>
#include <typeinfo>


namespace qtutils { namespace ui{

#define QTUTILS_RSBL_WND_POS_KEY				"/position"
#define QTUTILS_RSBL_WND_SIZE_KEY			"/size"
#define QTUTILS_RSBL_WND_IS_MAXIMIZED_KEY	"/isMaximized"
#define QTUTILS_RSBL_WND_IS_MINIMIZED_KEY	"/isMinimized"
#define QTUTILS_RSBL_WND_TBIND_KEY			"/tabindex"


template <typename WidgetType>
template<typename... Targs>
ResizibleWindow<WidgetType>::ResizibleWindow(Targs... a_args)
	:
	  WidgetType(a_args...),
	  m_settingsKey(typeid(this).name())
{
    m_flags.all = MONITOR_INIT_BITS;
}


template <typename WidgetType>
ResizibleWindow<WidgetType>::~ResizibleWindow()
{
    HideCloseEvent();
}


template <typename WidgetType>
const QString& ResizibleWindow<WidgetType>::settingsKey()const
{
    return m_settingsKey;
}


template <typename WidgetType>
inline void ResizibleWindow<WidgetType>::HideCloseEvent()
{
    if(m_flags.b.hideNotCalled){
        Settings aSettings;
        aSettings.setValue(m_settingsKey+QTUTILS_RSBL_WND_POS_KEY,WidgetType::pos());
        if(WidgetType::isMaximized()){
            aSettings.setValue(m_settingsKey+QTUTILS_RSBL_WND_IS_MAXIMIZED_KEY,true);
        }
        else if(WidgetType::isMinimized()){
            aSettings.setValue(m_settingsKey+QTUTILS_RSBL_WND_IS_MINIMIZED_KEY,true);
        }
        else{
            aSettings.setValue(m_settingsKey+QTUTILS_RSBL_WND_IS_MAXIMIZED_KEY,false);
            aSettings.setValue(m_settingsKey+QTUTILS_RSBL_WND_IS_MINIMIZED_KEY,false);
            aSettings.setValue(m_settingsKey+QTUTILS_RSBL_WND_SIZE_KEY,WidgetType::size());
        }
        m_flags.b2.hideCalledOrNot =  MONITOR_MAKE_BITS_POSITIVE;
    }
}


template <typename WidgetType>
void ResizibleWindow<WidgetType>::hideEvent(QHideEvent* a_event)
{
	HideCloseEvent();
	WidgetType::hideEvent(a_event);
}


template <typename WidgetType>
void ResizibleWindow<WidgetType>::closeEvent(QCloseEvent* a_event)
{
	HideCloseEvent();
	WidgetType::closeEvent(a_event);
}


template <typename WidgetType>
void ResizibleWindow<WidgetType>::showEvent(QShowEvent* a_event)
{
	Settings aSettings;
	bool bIsMaximized = false;
	bool bIsMinimized = false;
	
	WidgetType::showEvent(a_event);
	
	if(aSettings.contains(m_settingsKey+QTUTILS_RSBL_WND_POS_KEY)){
		const QPoint aPos = aSettings.value(m_settingsKey+QTUTILS_RSBL_WND_POS_KEY).toPoint();
		WidgetType::move(aPos);
	}
	
	if(aSettings.contains(m_settingsKey+QTUTILS_RSBL_WND_SIZE_KEY)){
		const QSize aSize = aSettings.value(m_settingsKey+QTUTILS_RSBL_WND_SIZE_KEY).toSize();
		WidgetType::resize(aSize);
	}
	
	
	if(aSettings.contains(m_settingsKey+QTUTILS_RSBL_WND_IS_MAXIMIZED_KEY)){
		bIsMaximized = aSettings.value(m_settingsKey+QTUTILS_RSBL_WND_IS_MAXIMIZED_KEY).toBool();
	}
	
	if(aSettings.contains(m_settingsKey+QTUTILS_RSBL_WND_IS_MINIMIZED_KEY)){
		bIsMinimized = aSettings.value(m_settingsKey+QTUTILS_RSBL_WND_IS_MINIMIZED_KEY).toBool();
	}
	
	
	if(bIsMaximized){
		WidgetType::showMaximized();
	}
	else if(bIsMinimized){
		WidgetType::showMinimized();
	}
		
}



}}  // namespace qtutils { namespace ui{


#endif  // #ifndef QTUTILS_INCLUDE_RESIZIBLE_WINDOW_IMPL_HPP
