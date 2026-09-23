

#pragma once 

#ifndef QTUTILS_INCLUDE_LOGINTYPEWINDOW_IMPL_HPP
#define QTUTILS_INCLUDE_LOGINTYPEWINDOW_IMPL_HPP


#ifndef QTUTILS_INCLUDE_LOGINTYPEWINDOW_HPP
#include <qtutils/ui/logintypewindow.hpp>
#endif

namespace qtutils { namespace ui{


#if defined(CPPUTILS_EMSCRIPTEN_IS_USED) || defined(QTUTILS_UI_FORCE_WEB_STYLE)

#else

template <typename WidgetType>
LoginTypeWindow<WidgetType>::~LoginTypeWindow()
{
}


template <typename WidgetType>
void LoginTypeWindow<WidgetType>::InitRaw()
{
    MakeSizeHint();
}


template <typename WidgetType>
void LoginTypeWindow<WidgetType>::MakeSizeHint()
{
    ResizibleWindow<WidgetType>::setFixedSize(ResizibleWindow<WidgetType>::sizeHint());
}

#endif


}}  // namespace qtutils { namespace ui{


#endif  //  #ifndef QTUTILS_INCLUDE_LOGINTYPEWINDOW_IMPL_HPP
