//
// file:            resizible_window.hpp
// path:			include/focust/ui/resizible_window.hpp
// created on:		2021 Dec 21
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_RESIZIBLE_WINDOW_HPP
#define QTUTILS_INCLUDE_RESIZIBLE_WINDOW_HPP


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
    
    const QString& settingsKey()const;
	    
protected:
    //void moveEvent(QMoveEvent *event) override;
    //void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
	void hideEvent(QHideEvent *event) override; 
	void closeEvent(QCloseEvent *event) override;
    bool event(QEvent* event) override;
	
private:
	inline void HideCloseEvent();
    
protected:
	QString		m_settingsKey;
    union{
        uint64_t all;
        struct{
            uint64_t  hideCalled : 1;
            uint64_t  hideNotCalled : 1;
            uint64_t  settingsKeyInited : 1;
            uint64_t  settingsKeyNotInited : 1;
            uint64_t  reserved01 : 60;
        }b;
        struct{
            uint64_t  hideCalledOrNot : 2;
            uint64_t  settingsKeyInitedOrNot : 2;
            uint64_t  reserved01 : 60;
        }b2;
    }m_flags;
};


}}  // namespace qtutils { namespace ui{


#ifndef QTUTILS_INCLUDE_RESIZIBLE_WINDOW_IMPL_HPP
#include "resizible_window.impl.hpp"
#endif


#endif  // #ifndef QTUTILS_INCLUDE_RESIZIBLE_WINDOW_HPP
