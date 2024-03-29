//
// file:            sizeresource.hpp
// path:			include/qtutils/ui/sizeresource.hpp
// created on:		2022 Aug 19
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_SIZERESOURCE_HPP
#define QTUTILS_INCLUDE_QTUTILS_UI_SIZERESOURCE_HPP

#include <qtutils/export_symbols.h>

namespace qtutils{ namespace ui{


template <typename NumTypeX, typename NumTypeY=NumTypeX>
class SizeResource
{
public:
    template <typename TypeNumVal>
    inline int getPixelsX(const TypeNumVal& a_value)const;

    template <typename TypeNumVal>
    inline int getPixelsY(const TypeNumVal& a_value)const;

public:
    NumTypeX     m_ratioX;
    NumTypeY     m_ratioY;
};


template <typename WidgetType,typename NumTypeX=int,typename NumTypeY=NumTypeX>
class SizeResourceUser : public WidgetType
{
public:
    template<typename... Targs>
    SizeResourceUser(const SizeResource<NumTypeX,NumTypeY>* a_pSR, Targs... a_args);
    virtual ~SizeResourceUser() override;

public:
    const SizeResource<NumTypeX,NumTypeY>*const  m_pSR;
};


template <typename WidgetType>
using SizeResourceUserI = SizeResourceUser<WidgetType>;

template <typename WidgetType>
using SizeResourceUserD = SizeResourceUser<WidgetType,double>;


}}  //  namespace qtutils{ namespace ui{


#ifndef QTUTILS_INCLUDE_QTUTILS_UI_SIZERESOURCE_IMPL_HPP
#include "sizeresource.impl.hpp"
#endif


#endif  //  #ifndef QTUTILS_INCLUDE_QTUTILS_UI_SIZERESOURCE_HPP
