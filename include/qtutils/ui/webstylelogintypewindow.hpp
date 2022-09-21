//
// file:            webstylelogintypewindow.hpp
// path:			include/qtutils/ui/webstylelogintypewindow.hpp
// created on:		2022 Jan 18
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_WEBSTYLELOGINTYPEWINDOW_HPP
#define QTUTILS_INCLUDE_WEBSTYLELOGINTYPEWINDOW_HPP


#include <qtutils/export_symbols.h>
#include <qtutils/ui/resiziblewindow.hpp>
#include <type_traits>
#include <qtutils/disable_utils_warnings.h>
#include <QWidget>


namespace qtutils { namespace ui{

template <typename WidgetType>
class WebStyleLoginTypeWindow : public ::qtutils::ui::ResizibleWindowRaw<QWidget>
{
    static_assert( ::std::is_base_of<QWidget,WidgetType>(), "WidgetType should be child of QWidget" );
public:
    template<typename... Targs>
	WebStyleLoginTypeWindow(Targs... a_args);
    virtual ~WebStyleLoginTypeWindow() override;
    WidgetType* childWdg();
    
    virtual void InitRaw() override;
    void MakeSizeHint();
	
protected:
	void resizeEvent(QResizeEvent *event) override;
	
protected:
	WidgetType		m_ltWnd;
};

}}  // namespace qtutils { namespace ui{


#ifndef QTUTILS_INCLUDE_WEBSTYLELOGINTYPEWINDOW_IMPL_HPP
#include "webstylelogintypewindow.impl.hpp"
#endif


#endif  // #ifndef QTUTILS_INCLUDE_WEBSTYLELOGINTYPEWINDOW_HPP
