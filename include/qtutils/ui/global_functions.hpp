//
// file:            global_functions.hpp
// path:			include/focust/ui/global_functions.hpp
// created on:		2022 Jan 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once


#include <qtutils/export_symbols.h>
#include <qtutils/disable_utils_warnings.h>
#include <cinternal/disable_compiler_warnings.h>
#include <QWidget>
#include <QLabel>
#include <QMessageBox>
#include <QString>
#include <cinternal/undisable_compiler_warnings.h>


namespace qtutils { namespace ui{


QTUTILS_EXPORT bool isSameWidgetOrChild(QWidget* a_pPossibleParent, QWidget* a_pPossibleChild);
QTUTILS_EXPORT void TakeFrameOfWindow(QWidget* a_pWindow);
QTUTILS_EXPORT void AssignFrameToWindow(QWidget* a_pWindow);
QTUTILS_EXPORT void SetProperParamsToImagedLbl(QLabel* a_pLbl);
QTUTILS_EXPORT void ShowMessageBox(const QMessageBox::Icon& a_icon, const QString a_title, const QString a_text, QMessageBox::StandardButtons a_buttons = QMessageBox::NoButton, QWidget* a_pParent=nullptr);


}}  // namespace qtutils { namespace ui{
