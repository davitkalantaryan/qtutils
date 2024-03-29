//
// repo:            qtutils
// file:            webstylelogintypewindow.hpp
// path:			include/qtutils/ui/webstylelogintypewindow.hpp
// created on:		2023 Jul 04
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_WEBSTYLELOGINTYPEWINDOW_IMPL_HPP
#define QTUTILS_INCLUDE_WEBSTYLELOGINTYPEWINDOW_IMPL_HPP


#ifndef QTUTILS_INCLUDE_WEBSTYLELOGINTYPEWINDOW_HPP
#include "webstylelogintypewindow.hpp"
#endif



namespace qtutils { namespace ui{


template <typename WidgetType>
template<typename... Targs>
WebStyleLoginTypeWindow<WidgetType>::WebStyleLoginTypeWindow(Targs... a_args)
	:
	  m_ltWnd(a_args...)
{
    QWidget* pParent = m_ltWnd.parentWidget();
    m_ltWnd.setParent(this);
    ::qtutils::ui::ResizibleWindowRaw<QWidget>::setParent(pParent);
}


template <typename WidgetType>
WebStyleLoginTypeWindow<WidgetType>::~WebStyleLoginTypeWindow()
{
}


template <typename WidgetType>
WidgetType* WebStyleLoginTypeWindow<WidgetType>::childWdg()
{
    return &m_ltWnd;
}


#define QTUTILS_WS_LT_UPPER_MARGIN	25


template <typename WidgetType>
void WebStyleLoginTypeWindow<WidgetType>::InitRaw()
{
    MakeSizeHint();
}


template <typename WidgetType>
void WebStyleLoginTypeWindow<WidgetType>::MakeSizeHint()
{
    //m_ltWnd.setFixedSize()
    const QSize childSizeHint = m_ltWnd.sizeHint();
    //::qtutils::ui::ResizibleWindowRaw<QWidget>::setMinimumHeight(childSizeHint.height()+2*QTUTILS_WS_LT_UPPER_MARGIN);
    //::qtutils::ui::ResizibleWindowRaw<QWidget>::setMinimumWidth(childSizeHint.width()+2);
    ::qtutils::ui::ResizibleWindowRaw<QWidget>::setMinimumSize(childSizeHint);
    m_ltWnd.setFixedSize(childSizeHint);
}


template <typename WidgetType>
void WebStyleLoginTypeWindow<WidgetType>::resizeEvent(QResizeEvent* a_event)
{
	::qtutils::ui::ResizibleWindowRaw<QWidget>::resizeEvent(a_event);
	
	int nMoveX=0,nMoveY=0;
    const QSize ltwSizeHint = m_ltWnd.sizeHint();
    const QSize& newSize = a_event->size();
    const int cnXltw = ltwSizeHint.width();
    const int cnYltw = ltwSizeHint.height();
    const int cnXnew = newSize.width();
    const int cnYnew = newSize.height();
    
    if(cnXltw<cnXnew){
        m_ltWnd.setFixedWidth(cnXltw);
        nMoveX = (cnXnew-cnXltw)/2;
    }
    else{
         m_ltWnd.setFixedWidth(cnXnew);
    }
    
    if(cnYltw<cnYnew){
        m_ltWnd.setFixedHeight(cnYltw);
        const int nYdiff = cnYnew-cnYltw;
        if(nYdiff>QTUTILS_WS_LT_UPPER_MARGIN){
            nMoveY = QTUTILS_WS_LT_UPPER_MARGIN;
        }
        else{
            nMoveY = nYdiff/2;
        }
    }
    else{
         m_ltWnd.setFixedHeight(cnYnew);
    }
    
    m_ltWnd.move(nMoveX,nMoveY);
}


}}  // namespace qtutils { namespace ui{


#endif  // #ifndef QTUTILS_INCLUDE_WEBSTYLELOGINTYPEWINDOW_IMPL_HPP
