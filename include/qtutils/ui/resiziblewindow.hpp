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
class ResizibleWindow : public WidgetType
{    
public:
	template<typename... Targs>
    ResizibleWindow(Targs... a_args);
    virtual ~ResizibleWindow() override;
    
    virtual void Init();
    const QString& settingsKey()const;
	    
protected:
    void showEvent(QShowEvent *event) override;
	void hideEvent(QHideEvent *event) override; 
	void closeEvent(QCloseEvent *event) override;
	
private:
	inline void HideCloseEvent();
    
protected:
	QString		m_settingsKey;
    union{
        uint64_t all;
        struct{
            uint64_t  hideCalled : 1;
            uint64_t  hideNotCalled : 1;
            uint64_t  reserved01 : 62;
        }b;
        struct{
            uint64_t  hideCalledOrNot : 2;
            uint64_t  reserved01 : 62;
        }b2;
    }m_flags;
};


}}  // namespace qtutils { namespace ui{


#ifndef QTUTILS_INCLUDE_RESIZIBLEWINDOW_IMPL_HPP
#include "resiziblewindow.impl.hpp"
#endif


#endif  // #ifndef QTUTILS_INCLUDE_RESIZIBLEWINDOW_HPP
