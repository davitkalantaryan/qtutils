//
// file:            logintypewindow.hpp
// path:			include/qtutils/ui/logintypewindow.hpp
// created on:		2022 Jan 18
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_LOGINTYPEWINDOW_HPP
#define QTUTILS_INCLUDE_LOGINTYPEWINDOW_HPP


#include <qtutils/qtutils_internal_header.h>
#include <qtutils/ui/resizible_window.hpp>
#if defined(CPPUTILS_EMSCRIPTEN_IS_USED) || defined(QTUTILS_UI_FORCE_WEB_STYLE)
#include <qtutils/ui/webstylelogintypewindow.hpp>
#else
#include <qtutils/ui/resizible_window.hpp>
#endif


namespace qtutils { namespace ui{

#if defined(CPPUTILS_EMSCRIPTEN_IS_USED) || defined(QTUTILS_UI_FORCE_WEB_STYLE)

#define QTUTILS_HANDLE_WIDGETP()        parentWidget()
#define QTUTILS_HANDLE_WIDGETC(_ww)     ((_ww)->childWdg())
template <typename WidgetType >
using LoginTypeWindow = WebStyleLoginTypeWindow<WidgetType>;

#else

#define QTUTILS_HANDLE_WIDGETP()        ( this )
#define QTUTILS_HANDLE_WIDGETC(_ww)     (_ww)
template <typename WidgetType >
using LoginTypeWindow = ResizibleWindow<WidgetType>;

#endif


}}  // namespace qtutils { namespace ui{


#endif  // #ifndef QTUTILS_INCLUDE_LOGINTYPEWINDOW_HPP
