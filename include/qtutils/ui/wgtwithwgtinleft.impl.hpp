//
// file:            wgtwithwgtinleft.impl.hpp
// path:			include/qtutils/ui/wgtwithwgtinleft.impl.hpp
// created on:		2022 Sep 26
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_WGTWITHWGTINLEFT_IMPL_HPP
#define QTUTILS_INCLUDE_QTUTILS_UI_WGTWITHWGTINLEFT_IMPL_HPP

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_WGTWITHWGTINLEFT_HPP
#include "wgtwithwgtinleft.hpp"
#endif

#include <type_traits>

namespace qtutils { namespace ui{


template <typename WidgetType, typename LeftWgtType>
WgtWithWgtInLeft<WidgetType,LeftWgtType>::~WgtWithWgtInLeft()
{
    delete m_pWgtInLeft;
}



template <typename WidgetType, typename LeftWgtType>
template<typename... Targs>
WgtWithWgtInLeft<WidgetType,LeftWgtType>::WgtWithWgtInLeft(Targs... a_args)
    :
      ::qtutils::ui::SizeApplyWindow<WidgetType>(a_args...),
      m_pWgtInLeft(nullptr)
{
    //#if ::std::is_bse_of<>
    //#endif
}


template <typename WidgetType, typename LeftWgtType>
void WgtWithWgtInLeft<WidgetType,LeftWgtType>::ApplyNewSize(const QSize& a_newSize)
{
    if(m_pWgtInLeft){
        int eyeSizeLn;
        const int newHeight = a_newSize.height();
        if(newHeight>2){
            eyeSizeLn = newHeight-2;
        }
        else{
            eyeSizeLn = newHeight-2;
        }
        m_pWgtInLeft->resize(eyeSizeLn,eyeSizeLn);
        m_pWgtInLeft->move(1,1);
    }
}


template <typename WidgetType, typename LeftWgtType>
LeftWgtType* WgtWithWgtInLeft<WidgetType,LeftWgtType>::wgtInLeft()const
{
    return m_pWgtInLeft;
}


template <typename WidgetType, typename LeftWgtType>
void WgtWithWgtInLeft<WidgetType,LeftWgtType>::SetWgtInLeft(LeftWgtType* a_pWgtInLeft)
{
    delete m_pWgtInLeft;
    m_pWgtInLeft = a_pWgtInLeft;
    if(m_pWgtInLeft){
        m_pWgtInLeft->setParent(this);
        QObject::connect(m_pWgtInLeft,&QObject::destroyed,this,[this](QObject* a_pLftWgt){
            if(static_cast<QObject*>(m_pWgtInLeft)==a_pLftWgt){
                m_pWgtInLeft=nullptr;
            }
        });
    }
}




}} // namespace qtutils { namespace ui{


#endif  // #ifndef QTUTILS_INCLUDE_QTUTILS_UI_WGTWITHWGTINLEFT_IMPL_HPP
