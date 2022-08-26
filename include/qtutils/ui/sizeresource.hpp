//
// file:            sizeresource.hpp
// path:			include/qtutils/ui/sizeresource.hpp
// created on:		2022 Aug 19
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_SIZERESOURCE_HPP
#define QTUTILS_INCLUDE_QTUTILS_UI_SIZERESOURCE_HPP

#include <qtutils/internal_header.h>

namespace qtutils{ namespace ui{


template <typename NumType>
class SizeResource
{
public:
    template <typename TypeNumVal>
    inline int getPixelsX(const TypeNumVal& a_value)const;

    template <typename TypeNumVal>
    inline int getPixelsY(const TypeNumVal& a_value)const;

public:
    NumType     m_ratioX;
    NumType     m_ratioY;
};


template <typename WidgetType,typename NumType=int>
class SizeResourceUser : public WidgetType
{
public:
    template<typename... Targs>
    SizeResourceUser(const SizeResource<NumType>* a_pSR, Targs... a_args);
    virtual ~SizeResourceUser() override;

public:
    const SizeResource<NumType>*const  m_pSR;
};


}}  //  namespace qtutils{ namespace ui{


#ifndef QTUTILS_INCLUDE_QTUTILS_UI_SIZERESOURCE_IMPL_HPP
#include "sizeresource.impl.hpp"
#endif


#endif  //  #ifndef QTUTILS_INCLUDE_QTUTILS_UI_SIZERESOURCE_HPP
