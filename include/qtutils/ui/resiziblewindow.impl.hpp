//
// file:            resizible_window.impl.hpp
// path:			include/focust/ui/resizible_window.impl.hpp
// created on:		2021 Dec 21
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_RESIZIBLEWINDOW_IMPL_HPP
#define QTUTILS_INCLUDE_RESIZIBLEWINDOW_IMPL_HPP


#ifndef QTUTILS_INCLUDE_RESIZIBLEWINDOW_HPP
#include "resiziblewindow.hpp"
#endif

#include <cpputils/flagshelper.h>
#include <qtutils/core/settings.hpp>
#include <typeinfo>
#ifdef QTUTILS_MAKE_DEBUG
#include <qtutils/core/logger.hpp>
#endif


namespace qtutils { namespace ui{

#define QTUTILS_RSBL_WND_POS_KEY				"/position"
#define QTUTILS_RSBL_WND_SIZE_KEY			"/size"
#define QTUTILS_RSBL_WND_IS_MAXIMIZED_KEY	"/isMaximized"
#define QTUTILS_RSBL_WND_IS_MINIMIZED_KEY	"/isMinimized"
#define QTUTILS_RSBL_WND_TBIND_KEY			"/tabindex"


template <typename WidgetType>
uint64_t ResizibleWindowRaw<WidgetType>::sn_numberOfInstances = 0;


template <typename WidgetType>
template<typename... Targs>
ResizibleWindowRaw<WidgetType>::ResizibleWindowRaw(Targs... a_args)
	:
	  WidgetType(a_args...)
{
    m_flags.all = CPPUTILS_INIT_BITS;
	m_flags.b.instanceNumber = (sn_numberOfInstances++);
}


template <typename WidgetType>
ResizibleWindowRaw<WidgetType>::~ResizibleWindowRaw()
{
    HideCloseEvent();
}


template <typename WidgetType>
void ResizibleWindowRaw<WidgetType>::Init2()
{
	if(m_flags.b.initNotCalled){
        m_flags.b2.initCalledOrNot = CPPUTILS_MAKE_BITS_TRUE;
		m_settingsKey = typeid(*this).name()+QString::number(m_flags.b.instanceNumber);
		this->InitRaw();
	}
}


template <typename WidgetType>
void ResizibleWindowRaw<WidgetType>::InitRaw()
{
	m_settingsKey = typeid(*this).name()+QString::number(m_flags.b.instanceNumber);
}


template <typename WidgetType>
void ResizibleWindowRaw<WidgetType>::InitAndShow()
{
    if(InitAndShowBase()){
        WidgetType::show();
    }
}


template <typename WidgetType>
inline bool ResizibleWindowRaw<WidgetType>::InitAndShowBase()
{
    bool bIsMinimized = false;
    if(m_flags.b.initNotCalled){
        bIsMinimized = true;
        m_flags.b2.initCalledOrNot = CPPUTILS_MAKE_BITS_TRUE;
        m_settingsKey = typeid(*this).name()+QString::number(m_flags.b.instanceNumber);
        this->InitRaw();
    }

    Settings aSettings;
    bool bIsMaximized = false;

    if(aSettings.contains(m_settingsKey+QTUTILS_RSBL_WND_POS_KEY)){
        const QPoint aPos = aSettings.value(m_settingsKey+QTUTILS_RSBL_WND_POS_KEY).toPoint();
        WidgetType::move(aPos);
    }

    if(aSettings.contains(m_settingsKey+QTUTILS_RSBL_WND_IS_MAXIMIZED_KEY)){
        bIsMaximized = aSettings.value(m_settingsKey+QTUTILS_RSBL_WND_IS_MAXIMIZED_KEY).toBool();
    }

    if(bIsMinimized){
        if(aSettings.contains(m_settingsKey+QTUTILS_RSBL_WND_IS_MINIMIZED_KEY)){
            bIsMinimized = aSettings.value(m_settingsKey+QTUTILS_RSBL_WND_IS_MINIMIZED_KEY).toBool();
        }
    }

    if(bIsMaximized){
        if(!(WidgetType::isMaximized())){
            WidgetType::showMaximized();
            return false;
        }
    }
    else if(bIsMinimized){
        if(!(WidgetType::isMinimized())){
            WidgetType::showMinimized();
            return false;
        }
    }
    return true;
}


template <typename WidgetType>
const QString& ResizibleWindowRaw<WidgetType>::settingsKey()const
{
    return m_settingsKey;
}


template <typename WidgetType>
inline void ResizibleWindowRaw<WidgetType>::HideCloseEvent()
{
    //if(m_flags.b.hideNotCalled)
	{
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
        m_flags.b2.hideCalledOrNot =  CPPUTILS_MAKE_BITS_TRUE;
    }
}


template <typename WidgetType>
void ResizibleWindowRaw<WidgetType>::showEvent(QShowEvent* a_event)
{
    InitAndShowBase();
    WidgetType::showEvent(a_event);
}


template <typename WidgetType>
void ResizibleWindowRaw<WidgetType>::closeEvent(QCloseEvent* a_event)
{
	HideCloseEvent();
	WidgetType::closeEvent(a_event);
}


template <typename WidgetType>
void ResizibleWindowRaw<WidgetType>::hideEvent(QHideEvent* a_event)
{
	HideCloseEvent();
	WidgetType::hideEvent(a_event);
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

template <typename WidgetType>
template<typename... Targs>
ResizibleWindow<WidgetType>::ResizibleWindow(Targs... a_args)
	:
	  ResizibleWindowRaw<WidgetType>(a_args...)
{
}


template <typename WidgetType>
ResizibleWindow<WidgetType>::~ResizibleWindow()
{
}


template <typename WidgetType>
void ResizibleWindow<WidgetType>::showEvent(QShowEvent* a_event)
{
	Settings aSettings;	
	ResizibleWindowRaw<WidgetType>::showEvent(a_event);
	
	if(aSettings.contains(ResizibleWindowRaw<WidgetType>::m_settingsKey+QTUTILS_RSBL_WND_SIZE_KEY)){
		const QSize aSize = aSettings.value(ResizibleWindowRaw<WidgetType>::m_settingsKey+QTUTILS_RSBL_WND_SIZE_KEY).toSize();
		ResizibleWindowRaw<WidgetType>::resize(aSize);
	}
}


}}  // namespace qtutils { namespace ui{


#endif  // #ifndef QTUTILS_INCLUDE_RESIZIBLEWINDOW_IMPL_HPP
