//
// file:            spinnerbox.hpp
// path:			include/qtutils/ui/spinnerbox.hpp
// created on:		2022 Apr 01
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_SPINNERBOX_HPP
#define QTUTILS_INCLUDE_QTUTILS_UI_SPINNERBOX_HPP

#include <qtutils/qtutils_internal_header.h>
#include <qtutils/disable_utils_warnings.h>
#include <QLabel>


namespace qtutils { namespace ui{

class CPPUTILS_DLL_PRIVATE SpinnerBox_p;

class SpinnerBox QTUTILS_EXPORT : public QLabel
{
public:
	template<typename... Targs>
    SpinnerBox(Targs... a_args);
    virtual ~SpinnerBox() override;
	
	void StartSpinning();
	void StopSpinning();
	
private:
	static SpinnerBox_p* CreateSpinnerBox_p(SpinnerBox* a_pThis);
        
private:
    SpinnerBox_p*const m_spbox_data_p;
};


}}  // namespace qtutils { namespace ui{


#ifndef QTUTILS_INCLUDE_QTUTILS_UI_SPINNERBOX_IMPL_HPP
#include "spinnerbox.impl.hpp"
#endif


#endif  // #ifndef QTUTILS_INCLUDE_QTUTILS_UI_SPINNERBOX_HPP
