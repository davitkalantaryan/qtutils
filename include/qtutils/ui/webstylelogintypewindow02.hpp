//
// file:            webstylelogintypewindow02.hpp
// path:			include/qtutils/ui/webstylelogintypewindow02.hpp
// created on:		2023 Jul 04
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_WEBSTYLELOGINTYPEWINDOW02_HPP
#define QTUTILS_INCLUDE_WEBSTYLELOGINTYPEWINDOW02_HPP


#include <qtutils/export_symbols.h>
#include <qtutils/ui/resiziblewindow.hpp>
#include <type_traits>
#include <qtutils/disable_utils_warnings.h>
#include <QWidget>
#include <QSize>


namespace qtutils { namespace ui{

template <typename WidgetType, typename ParentWidgetType=QWidget>
class WebStyleLoginTypeWindow02 : public ::qtutils::ui::ResizibleWindowRaw<ParentWidgetType>
{
    static_assert( ::std::is_base_of<QWidget,WidgetType>(), "WidgetType should be child of QWidget" );
    static_assert( ::std::is_base_of<QWidget,ParentWidgetType>(), "ParentWidgetType should be child of QWidget" );
public:
    template<typename... Targs>
    WebStyleLoginTypeWindow02(int,Targs... a_args);
    template<typename... Targs>
    WebStyleLoginTypeWindow02(double,Targs... a_args);
    virtual ~WebStyleLoginTypeWindow02() override;
    WidgetType* childWdg();
    
    virtual void InitRaw() override;
    void MakeSizeHint();
    void UseSizeHint(const QSize& a_sizeHint);
    void UnuseSizeHint();
	
protected:
	void resizeEvent(QResizeEvent *event) override;
	
protected:
	WidgetType		m_ltWnd;
    QSize           m_ltwSizeHint;
    bool            m_bHasSizeHint;
};

}}  // namespace qtutils { namespace ui{


#ifndef QTUTILS_INCLUDE_WEBSTYLELOGINTYPEWINDOW02_IMPL_HPP
#include "webstylelogintypewindow02.impl.hpp"
#endif


#endif  // #ifndef QTUTILS_INCLUDE_WEBSTYLELOGINTYPEWINDOW02_HPP
