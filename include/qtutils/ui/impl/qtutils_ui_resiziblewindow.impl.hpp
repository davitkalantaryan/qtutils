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
#include <qtutils/ui/resiziblewindow.hpp>
#endif

#ifdef QTUTILS_MAKE_DEBUG
#include <qtutils/core/logger.hpp>
#endif

#include <qtutils/ui/global_functions.hpp>
#include <cinternal/disable_compiler_warnings.h>
#include <typeinfo>
#include <qtutils/disable_utils_warnings.h>
#include <QSettings>
#include <cinternal/undisable_compiler_warnings.h>


namespace qtutils { namespace ui{


#define QTUTILS_RSBL_WND_POS_KEY			"/position"
#define QTUTILS_RSBL_WND_SIZE_KEY			"/size"
#define QTUTILS_RSBL_WND_IS_MAXIMIZED_KEY	"/isMaximized"
#define QTUTILS_RSBL_WND_IS_MINIMIZED_KEY	"/isMinimized"
#define QTUTILS_RSBL_WND_TBIND_KEY			"/tabindex"


template <typename WidgetType>
uint64_t ResizibleWindow<WidgetType>::sm_numberOfInstances = 0;


template <typename WidgetType>
template<typename... Targs>
ResizibleWindow<WidgetType>::ResizibleWindow(Targs... a_args)
	:
    WidgetType(a_args...),
    m_instanceNumber(sm_numberOfInstances++)
{
    m_flags.wr_all = CPPUTILS_BISTATE_MAKE_ALL_BITS_FALSE;
}


template <typename WidgetType>
inline void ResizibleWindow<WidgetType>::saveSizesInline() const
{
    if(m_flags.rd.hasCloseAfterShow_false){
        m_flags.wr.hasCloseAfterShow = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
        QSettings aSettings;
        aSettings.setValue(m_settingsKey+QTUTILS_RSBL_WND_POS_KEY,WidgetType::pos());
        aSettings.setValue(m_settingsKey+QTUTILS_RSBL_WND_SIZE_KEY,WidgetType::size());
        if(WidgetType::isMaximized()){
            aSettings.setValue(m_settingsKey+QTUTILS_RSBL_WND_IS_MAXIMIZED_KEY,true);
            aSettings.setValue(m_settingsKey+QTUTILS_RSBL_WND_IS_MINIMIZED_KEY,false);
        }
        else if(WidgetType::isMinimized()){
            aSettings.setValue(m_settingsKey+QTUTILS_RSBL_WND_IS_MAXIMIZED_KEY,false);
            aSettings.setValue(m_settingsKey+QTUTILS_RSBL_WND_IS_MINIMIZED_KEY,true);
        }
        else{
            aSettings.setValue(m_settingsKey+QTUTILS_RSBL_WND_IS_MAXIMIZED_KEY,false);
            aSettings.setValue(m_settingsKey+QTUTILS_RSBL_WND_IS_MINIMIZED_KEY,false);
        }
    }  //  if(m_flags.rd.hasCloseAfterShow_false){
}


template <typename WidgetType>
bool ResizibleWindow<WidgetType>::LoadSizesBecauseOfFirstShowCallInline(bool a_bFromShow)
{
    if(m_flags.rd.loadSizesCalled_false){
        m_flags.wr.loadSizesCalled = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
        m_settingsKey = typeid(*this).name()+QString::number(m_instanceNumber);
        const QSettings aSettings;
        if(aSettings.contains(m_settingsKey+QTUTILS_RSBL_WND_POS_KEY)){
            const QPoint aPos = aSettings.value(m_settingsKey+QTUTILS_RSBL_WND_POS_KEY).toPoint();
            QScreen* const pScreenOfPoint = ScreenOfWidgetPoint(*this,aPos);
            if(pScreenOfPoint){
                WidgetType::move(aPos);
            }
        }  //  if(aSettings.contains(m_settingsKey+QTUTILS_RSBL_WND_POS_KEY)){

        if(aSettings.contains(m_settingsKey+QTUTILS_RSBL_WND_SIZE_KEY)){
            const QSize aSize = aSettings.value(m_settingsKey+QTUTILS_RSBL_WND_SIZE_KEY).toSize();
            WidgetType::resize(aSize);
        }

        if(a_bFromShow){
            if(aSettings.contains(m_settingsKey+QTUTILS_RSBL_WND_IS_MINIMIZED_KEY)){
                const bool cbIsMinimized = aSettings.value(m_settingsKey+QTUTILS_RSBL_WND_IS_MINIMIZED_KEY).toBool();
                if(cbIsMinimized){
                    WidgetType::showMinimized();
                    return false;
                }
            }  //  if(aSettings.contains(m_settingsKey+QTUTILS_RSBL_WND_IS_MINIMIZED_KEY)){
            if(aSettings.contains(m_settingsKey+QTUTILS_RSBL_WND_IS_MAXIMIZED_KEY)){
                const bool cbIsMaximized = aSettings.value(m_settingsKey+QTUTILS_RSBL_WND_IS_MINIMIZED_KEY).toBool();
                if(cbIsMaximized){
                    WidgetType::showMaximized();
                    return false;
                }
            }  //  if(aSettings.contains(m_settingsKey+QTUTILS_RSBL_WND_IS_MAXIMIZED_KEY)){
            WidgetType::show();
            return false;
        }  //  if(a_bFromShow){
    }  //  if(m_flags.rd.loadSizesCalled_false){

    return true;
}


template <typename WidgetType>
const QString& ResizibleWindow<WidgetType>::settingsKey()const noexcept
{
    return m_settingsKey;
}


template <typename WidgetType>
void ResizibleWindow<WidgetType>::show()
{
    if(LoadSizesBecauseOfFirstShowCallInline(true)){
        WidgetType::show();
    }
}


template <typename WidgetType>
void ResizibleWindow<WidgetType>::showEvent(QShowEvent* a_event)
{
    m_flags.wr.hasCloseAfterShow = CPPUTILS_BISTATE_MAKE_BITS_FALSE;
    LoadSizesBecauseOfFirstShowCallInline(false);
    WidgetType::showEvent(a_event);
}


template <typename WidgetType>
void ResizibleWindow<WidgetType>::closeEvent(QCloseEvent* a_event)
{
    saveSizesInline();
	WidgetType::closeEvent(a_event);
}


template <typename WidgetType>
void ResizibleWindow<WidgetType>::hideEvent(QHideEvent* a_event)
{
    saveSizesInline();
	WidgetType::hideEvent(a_event);
}


}}  // namespace qtutils { namespace ui{


#endif  // #ifndef QTUTILS_INCLUDE_RESIZIBLEWINDOW_IMPL_HPP
