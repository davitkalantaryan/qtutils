//
// file:            global_functions.hpp
// path:			include/focust/ui/global_functions.hpp
// created on:		2022 Jan 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once


#include <qtutils/qtutils_internal_header.h>
#include <qtutils/disable_utils_warnings.h>
#include <QWidget>


namespace qtutils { namespace ui{

QTUTILS_EXPORT bool isSameWidgetOrChild(QWidget* a_pPossibleParent, QWidget* a_pPossibleChild);
QTUTILS_EXPORT void TakeFrameOfWindow(QWidget* a_pWindow);
QTUTILS_EXPORT void AssignFrameToWindow(QWidget* a_pWindow);


}}  // namespace qtutils { namespace test{
