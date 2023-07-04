//
// file:            webstylelogintypewindow02.impl.hpp
// path:			include/qtutils/ui/webstylelogintypewindow02.impl.hpp
// created on:		2023 Jul 04
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_WEBSTYLELOGINTYPEWINDOW02_IMPL_HPP
#define QTUTILS_INCLUDE_WEBSTYLELOGINTYPEWINDOW02_IMPL_HPP


#ifndef QTUTILS_INCLUDE_WEBSTYLELOGINTYPEWINDOW02_HPP
#include "webstylelogintypewindow02.hpp"
#endif



namespace qtutils { namespace ui{


template <typename WidgetType, typename ParentWidgetType>
template<typename... Targs>
WebStyleLoginTypeWindow02<WidgetType,ParentWidgetType>::WebStyleLoginTypeWindow02(int,Targs... a_args)
	:
	  m_ltWnd(a_args...)
{
    QWidget* pParent = m_ltWnd.parentWidget();
    m_bHasSizeHint = false;
    m_ltWnd.setParent(this);
    ::qtutils::ui::ResizibleWindowRaw<ParentWidgetType>::setParent(pParent);
}


template <typename WidgetType, typename ParentWidgetType>
template<typename... Targs>
WebStyleLoginTypeWindow02<WidgetType,ParentWidgetType>::WebStyleLoginTypeWindow02(double,Targs... a_args)
    :
      ::qtutils::ui::ResizibleWindowRaw<ParentWidgetType>(a_args...)
{
    QWidget* pParent = m_ltWnd.parentWidget();
    m_bHasSizeHint = false;
    m_ltWnd.setParent(this);
    ::qtutils::ui::ResizibleWindowRaw<ParentWidgetType>::setParent(pParent);
}


template <typename WidgetType, typename ParentWidgetType>
WebStyleLoginTypeWindow02<WidgetType,ParentWidgetType>::~WebStyleLoginTypeWindow02()
{
}


template <typename WidgetType, typename ParentWidgetType>
WidgetType* WebStyleLoginTypeWindow02<WidgetType,ParentWidgetType>::childWdg()
{
    return &m_ltWnd;
}


#define QTUTILS_WS_LT_UPPER_MARGIN02	25


template <typename WidgetType, typename ParentWidgetType>
void WebStyleLoginTypeWindow02<WidgetType,ParentWidgetType>::InitRaw()
{
    MakeSizeHint();
}


template <typename WidgetType, typename ParentWidgetType>
void WebStyleLoginTypeWindow02<WidgetType,ParentWidgetType>::MakeSizeHint()
{
    //m_ltWnd.setFixedSize()
    const QSize childSizeHint = m_bHasSizeHint?m_ltwSizeHint:m_ltWnd.sizeHint();
    //::qtutils::ui::ResizibleWindowRaw<ParentWidgetType>::setMinimumHeight(childSizeHint.height()+2*QTUTILS_WS_LT_UPPER_MARGIN02);
    //::qtutils::ui::ResizibleWindowRaw<ParentWidgetType>::setMinimumWidth(childSizeHint.width()+2);
    ::qtutils::ui::ResizibleWindowRaw<ParentWidgetType>::setMinimumSize(childSizeHint);
    m_ltWnd.setFixedSize(childSizeHint);
}


template <typename WidgetType, typename ParentWidgetType>
void WebStyleLoginTypeWindow02<WidgetType,ParentWidgetType>::UseSizeHint(const QSize& a_sizeHint)
{
    m_ltwSizeHint = a_sizeHint;
    m_bHasSizeHint= true;
    MakeSizeHint();
}


template <typename WidgetType, typename ParentWidgetType>
void WebStyleLoginTypeWindow02<WidgetType,ParentWidgetType>::UnuseSizeHint()
{
    m_bHasSizeHint= false;
    MakeSizeHint();
}


template <typename WidgetType, typename ParentWidgetType>
void WebStyleLoginTypeWindow02<WidgetType,ParentWidgetType>::resizeEvent(QResizeEvent* a_event)
{
    ::qtutils::ui::ResizibleWindowRaw<ParentWidgetType>::resizeEvent(a_event);
	
	int nMoveX=0,nMoveY=0;
    const QSize ltwSizeHint = m_bHasSizeHint?m_ltwSizeHint:m_ltWnd.sizeHint();
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
        if(nYdiff>QTUTILS_WS_LT_UPPER_MARGIN02){
            nMoveY = QTUTILS_WS_LT_UPPER_MARGIN02;
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


#endif  // #ifndef QTUTILS_INCLUDE_WEBSTYLELOGINTYPEWINDOW02_IMPL_HPP
