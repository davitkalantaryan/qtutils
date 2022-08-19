//
// file:            sizeresource.impl.hpp
// path:			include/qtutils/ui/sizeresource.impl.hpp
// created on:		2022 Aug 19
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_SIZERESOURCE_IMPL_HPP
#define QTUTILS_INCLUDE_QTUTILS_UI_SIZERESOURCE_IMPL_HPP

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_SIZERESOURCE_HPP
#include "sizeresource.hpp"
#endif

namespace qtutils{ namespace ui{


template <typename NumType>
template <typename TypeNumVal>
inline int SizeResource<NumType>::getPixelsX(const TypeNumVal& a_value)const
{
    return static_cast<int>(m_ratioX*a_value);
}


template <typename NumType>
template <typename TypeNumVal>
inline int SizeResource<NumType>::getPixelsY(const TypeNumVal& a_value)const
{
    return static_cast<int>(m_ratioY*a_value);
}


//
template <typename WidgetType,typename NumType>
template<typename... Targs>
SizeResourceUser<WidgetType,NumType>::SizeResourceUser(const SizeResource<NumType>* a_pSR, Targs... a_args)
    :
      WidgetType(a_args...),
      m_pSR(a_pSR)
{
}


template <typename WidgetType,typename NumType>
SizeResourceUser<WidgetType,NumType>::~SizeResourceUser()
{
}


}}  //  namespace qtutils{ namespace ui{



#endif  //  #ifndef QTUTILS_INCLUDE_QTUTILS_UI_SIZERESOURCE_IMPL_HPP
