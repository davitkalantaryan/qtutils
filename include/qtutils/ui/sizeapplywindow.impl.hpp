//
// file:            sizeapplywindow.impl.hpp
// path:			include/qtutils/ui/sizeapplywindow.impl.hpp
// created on:		2022 Aug 16
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_SIZEAPPLYWINDOW_IMPL_HPP
#define QTUTILS_INCLUDE_QTUTILS_UI_SIZEAPPLYWINDOW_IMPL_HPP

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_SIZEAPPLYWINDOW_HPP
#include "sizeapplywindow.hpp"
#endif

namespace qtutils { namespace ui{

template <typename WidgetType>
void SizeApplyWindow<WidgetType>::showEvent(QShowEvent* a_event)
{
    WidgetType::showEvent(a_event);
    this->ApplyNewSize(WidgetType::size());
}


template <typename WidgetType>
void SizeApplyWindow<WidgetType>::resizeEvent(QResizeEvent* a_event)
{
    WidgetType::resizeEvent(a_event);
    this->ApplyNewSize(a_event->size());
}


}} // namespace qtutils { namespace ui{


#endif
