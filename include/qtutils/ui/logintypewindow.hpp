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
#include <qtutils/ui/resiziblewindow.hpp>
#if defined(CPPUTILS_EMSCRIPTEN_IS_USED) || defined(QTUTILS_UI_FORCE_WEB_STYLE)
#include <qtutils/ui/webstylelogintypewindow.hpp>
#else
#include <qtutils/ui/resiziblewindow.hpp>
#endif


namespace qtutils { namespace ui{

#if defined(CPPUTILS_EMSCRIPTEN_IS_USED) || defined(QTUTILS_UI_FORCE_WEB_STYLE)

//#define QTUTILS_HANDLE_WIDGETP()        parentWidget()
//#define QTUTILS_HANDLE_WIDGETC(_ww)     ((_ww)->childWdg())
template <typename WidgetType >
using LoginTypeWindow = WebStyleLoginTypeWindow<WidgetType>;

#else

//#define QTUTILS_HANDLE_WIDGETP()        ( this )
//#define QTUTILS_HANDLE_WIDGETC(_ww)     (_ww)
template <typename WidgetType >
class LoginTypeWindow : public ResizibleWindowRaw<WidgetType>
{
public:
    using ResizibleWindowRaw<WidgetType>::ResizibleWindowRaw;
    virtual ~LoginTypeWindow() override;
    virtual void Init() override;
    void MakeSizeHint();
};

#endif


}}  // namespace qtutils { namespace ui{

#ifndef QTUTILS_INCLUDE_LOGINTYPEWINDOW_IMPL_HPP
#include "logintypewindow.impl.hpp"
#endif


#endif  // #ifndef QTUTILS_INCLUDE_LOGINTYPEWINDOW_HPP
