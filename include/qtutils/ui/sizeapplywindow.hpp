//
// file:            sizeapplywindow.hpp
// path:			include/qtutils/ui/sizeapplywindow.hpp
// created on:		2022 Aug 16
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_SIZEAPPLYWINDOW_HPP
#define QTUTILS_INCLUDE_QTUTILS_UI_SIZEAPPLYWINDOW_HPP


#include <qtutils/qtutils_internal_header.h>
#include <qtutils/disable_utils_warnings.h>
#include <QResizeEvent>
#include <QShowEvent>


namespace qtutils { namespace ui{


template <typename WidgetType>
class SizeApplyWindow : public WidgetType
{
public:
    using WidgetType::WidgetType;
    
protected:
    virtual void showEvent(QShowEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    virtual void ApplyNewSize(const QSize& a_newSize) = 0;
};


}}  // namespace qtutils { namespace ui{


#ifndef QTUTILS_INCLUDE_QTUTILS_UI_SIZEAPPLYWINDOW_IMPL_HPP
#include "sizeapplywindow.impl.hpp"
#endif



#endif  // #ifndef QTUTILS_INCLUDE_QTUTILS_UI_SIZEAPPLYWINDOW_HPP
