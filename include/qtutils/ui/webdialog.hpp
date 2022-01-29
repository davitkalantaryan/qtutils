//
// file:            webdialog.hpp
// path:			include/qtutils/ui/webdialog.hpp
// created on:		2022 Jan 19
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_WEBDIALOG_HPP
#define QTUTILS_INCLUDE_QTUTILS_UI_WEBDIALOG_HPP


#include <qtutils/qtutils_internal_header.h>
#include <type_traits>
#include <qtutils/disable_utils_warnings.h>
#include <QDialog>


namespace qtutils { namespace ui{

template <typename WidgetType>
class WebDialog : public WidgetType
{
    static_assert( ::std::is_base_of<QDialog,WidgetType>(), "WidgetType should be child of QDialog" );
public:
    template<typename... Targs>
	WebDialog(Targs... a_args);
    virtual ~WebDialog() override;
		
protected:
	QMetaObject::Connection		m_con1;
    QMetaObject::Connection     m_con2;
};

}}  // namespace qtutils { namespace ui{


#ifndef QTUTILS_INCLUDE_QTUTILS_UI_WEBDIALOG_IMPL_HPP
#include "webdialog.impl.hpp"
#endif


#endif  // #ifndef QTUTILS_INCLUDE_QTUTILS_UI_WEBDIALOG_HPP
