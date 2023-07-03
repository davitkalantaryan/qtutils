//
// file:            spinnerparent.hpp
// path:			include/qtutils/ui/spinnerparent.hpp
// created on:		2022 Apr 01
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_SPINNERPARENT_HPP
#define QTUTILS_INCLUDE_QTUTILS_UI_SPINNERPARENT_HPP

#include <qtutils/export_symbols.h>
#include <qtutils/ui/spinnerbox.hpp>
#include <cpputils/flagshelper.h>


namespace qtutils { namespace ui{


template <typename WidgetType >
class SpinnerParent : public WidgetType
{
public:
	template<typename... Targs>
    SpinnerParent(Targs... a_args);
    virtual ~SpinnerParent() override;
	
	void SetSpinnerMargins(int a_margins);
	void StartSpinning();
	void StopSpinning();
	
protected:
	void moveEvent(QMoveEvent *event) override;
	void resizeEvent(QResizeEvent *event) override;
	void hideEvent(QHideEvent *event) override;
	void showEvent(QShowEvent *event) override;
		        
protected:
    SpinnerBox	m_spinnerBox;
	int			m_margins;
    int         m_shouldSpin;
};


}}  // namespace qtutils { namespace ui{


#ifndef QTUTILS_INCLUDE_QTUTILS_UI_SPINNERPARENT_IMPL_HPP
#include "spinnerparent.impl.hpp"
#endif


#endif  // #ifndef QTUTILS_INCLUDE_QTUTILS_UI_SPINNERPARENT_HPP
