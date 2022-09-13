//
// file:            wgtwithlblinright.impl.hpp
// path:			include/qtutils/ui/wgtwithlblinright.impl.hpp
// created on:		2022 Aug 20
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_WGTWITHLBLINRIGHT_IMPL_HPP
#define QTUTILS_INCLUDE_QTUTILS_UI_WGTWITHLBLINRIGHT_IMPL_HPP

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_WGTWITHLBLINRIGHT_HPP
#include "wgtwithlblinright.hpp"
#endif

#include <type_traits>

namespace qtutils { namespace ui{


template <typename WidgetType, typename RightWgtType>
template<typename... Targs>
WgtWithWgtInRight<WidgetType,RightWgtType>::WgtWithWgtInRight(Targs... a_args)
    :
      ::qtutils::ui::SizeApplyWindow<WidgetType>(a_args...),
      m_pWgtInRight(nullptr)
{
    //#if ::std::is_bse_of<>
    //#endif
}


template <typename WidgetType, typename RightWgtType>
void WgtWithWgtInRight<WidgetType,RightWgtType>::ApplyNewSize(const QSize& a_newSize)
{
    if(m_pWgtInRight){
        int eyeSizeLn;
        const int newHeight = a_newSize.height();
        if(newHeight>2){
            eyeSizeLn = newHeight-2;
        }
        else{
            eyeSizeLn = newHeight-2;
        }
        m_pWgtInRight->resize(eyeSizeLn,eyeSizeLn);
        m_pWgtInRight->move(a_newSize.width()-eyeSizeLn-1,1);
    }
}


template <typename WidgetType, typename RightWgtType>
RightWgtType* WgtWithWgtInRight<WidgetType,RightWgtType>::wgtInRight()const
{
    return m_pWgtInRight;
}


template <typename WidgetType, typename RightWgtType>
void WgtWithWgtInRight<WidgetType,RightWgtType>::SetWgtInRight(RightWgtType* a_pWgtInRight)
{
    delete m_pWgtInRight;
    m_pWgtInRight = a_pWgtInRight;
    if(m_pWgtInRight){
        m_pWgtInRight->setParent(this);
        QObject::connect(m_pWgtInRight,&QObject::destroyed,this,[this](QObject* a_pRghtWgt){
            if(static_cast<QObject*>(m_pWgtInRight)==a_pRghtWgt){
                m_pWgtInRight=nullptr;
            }
        });
    }
}




/*//////////////////////////////////////////////////////////////////////////////////////////////*/

template <typename WidgetType>
template<typename... Targs>
WgtWithLblInRight<WidgetType>::WgtWithLblInRight(Targs... a_args)
    :
      ::qtutils::ui::SizeApplyWindow<WidgetType>(a_args...),
      m_pLabelInRight(nullptr)
{
    SetLabelInRight(new QLabel());
}


template <typename WidgetType>
WgtWithLblInRight<WidgetType>::~WgtWithLblInRight()
{
    delete m_pLabelInRight;
}


template <typename WidgetType>
void WgtWithLblInRight<WidgetType>::ApplyNewSize(const QSize& a_newSize)
{
    int eyeSizeLn;
    const int newHeight = a_newSize.height();
    if(newHeight>2){
        eyeSizeLn = newHeight-2;
    }
    else{
        eyeSizeLn = newHeight-2;
    }
    m_pLabelInRight->resize(eyeSizeLn,eyeSizeLn);
    m_pLabelInRight->move(a_newSize.width()-eyeSizeLn-1,1);

//#if ::std::is_bse_of<>
//#endif
}





//

template<typename... Targs>
Label01::Label01(const TypeClbk& a_lblPushClbk, Targs... a_args)
    :
      QLabel(a_args...),
      m_lblPushClbk(a_lblPushClbk?a_lblPushClbk:([](Label01*){}))
{
}


}} // namespace qtutils { namespace ui{


#endif  // #ifndef QTUTILS_INCLUDE_QTUTILS_UI_WGTWITHLBLINRIGHT_IMPL_HPP
