//
// file:            resizible_window.hpp
// path:			include/focust/ui/resizible_window.hpp
// created on:		2021 Dec 21
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_RESIZIBLEWINDOW_HPP
#define QTUTILS_INCLUDE_RESIZIBLEWINDOW_HPP


#include <qtutils/qtutils_internal_header.h>
#include <cpputils/flagshelper.h>
#include <stdint.h>
#include <qtutils/disable_utils_warnings.h>
#include <QCloseEvent>
#include <QHideEvent>
#include <QMoveEvent>
#include <QResizeEvent>
#include <QShowEvent>

namespace qtutils { namespace ui{


template <typename WidgetType>
class ResizibleWindowRaw : public WidgetType
{    
public:
	template<typename... Targs>
    ResizibleWindowRaw(Targs... a_args);
    virtual ~ResizibleWindowRaw() override;
    
    const QString& settingsKey()const;
	void InitAndShow();
	virtual void Init(); 
	    
protected:
	virtual void closeEvent(QCloseEvent *event) override;
	
private:
	inline void HideCloseEvent();
	void show();  // we disable show call
	
protected:
	QString		m_settingsKey;
    union{
        uint64_t all;
        struct{
            uint64_t  hideCalled : 1;
            uint64_t  hideNotCalled : 1;
			uint64_t  initCalled : 1;
            uint64_t  initNotCalled : 1;
            uint64_t  reserved01 : 60;
        }b;
        struct{
            uint64_t  hideCalledOrNot : 2;
			uint64_t  initCalledOrNot : 2;
            uint64_t  reserved01 : 60;
        }b2;
    }m_flags;
};


template <typename WidgetType>
class ResizibleWindow : public ResizibleWindowRaw<WidgetType>
{
public:
    template<typename... Targs>
    ResizibleWindow(Targs... a_args);
    virtual ~ResizibleWindow() override;
    
protected:
    virtual void showEvent(QShowEvent *event) override;
};


}}  // namespace qtutils { namespace ui{


#ifndef QTUTILS_INCLUDE_RESIZIBLEWINDOW_IMPL_HPP
#include "resiziblewindow.impl.hpp"
#endif


#endif  // #ifndef QTUTILS_INCLUDE_RESIZIBLEWINDOW_HPP
