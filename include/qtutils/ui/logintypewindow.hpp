//
// file:            logintypewindow.hpp
// path:			include/qtutils/ui/logintypewindow.hpp
// created on:		2022 Jan 18
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_LOGINTYPEWINDOW_HPP
#define QTUTILS_INCLUDE_LOGINTYPEWINDOW_HPP


#include <qtutils/export_symbols.h>
#include <qtutils/ui/resiziblewindow.hpp>
#if defined(CPPUTILS_EMSCRIPTEN_IS_USED) || defined(QTUTILS_UI_FORCE_WEB_STYLE)
#include <qtutils/ui/webstylelogintypewindow.hpp>
#else
#include <qtutils/ui/resiziblewindow.hpp>
#endif


namespace qtutils { namespace ui{

#if defined(CPPUTILS_EMSCRIPTEN_IS_USED) || defined(QTUTILS_UI_FORCE_WEB_STYLE)

#define QTUTILS_LOGIN_WND_PARENT(_lw)   ((_lw)->parentWidget())
#define QTUTILS_LOGIN_WND_WIDGETC(_lw)  ((_lw)->childWdg())
template <typename WidgetType >
using LoginTypeWindow = WebStyleLoginTypeWindow<WidgetType>;

#else

#define QTUTILS_LOGIN_WND_PARENT(_lw)     (_lw)
#define QTUTILS_LOGIN_WND_WIDGETC(_lw)    (_lw)
template <typename WidgetType >
class LoginTypeWindow : public ResizibleWindowRaw<WidgetType>
{
public:
    using ResizibleWindowRaw<WidgetType>::ResizibleWindowRaw;
    virtual ~LoginTypeWindow() override;
    void MakeSizeHint();
protected:
	virtual void InitRaw() override;
};

#endif


}}  // namespace qtutils { namespace ui{

#ifndef QTUTILS_INCLUDE_LOGINTYPEWINDOW_IMPL_HPP
#include "logintypewindow.impl.hpp"
#endif


#endif  // #ifndef QTUTILS_INCLUDE_LOGINTYPEWINDOW_HPP
