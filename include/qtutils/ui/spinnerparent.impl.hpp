//
// file:            spinnerparent.impl.hpp
// path:			include/qtutils/ui/spinnerparent.impl.hpp
// created on:		2022 Apr 01
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_SPINNERPARENT_IMPL_HPP
#define QTUTILS_INCLUDE_QTUTILS_UI_SPINNERPARENT_IMPL_HPP

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_SPINNERPARENT_HPP
#include "spinnerparent.hpp"
#endif

#include <qtutils/ui/global_functions.hpp>

namespace qtutils { namespace ui{

template <typename WidgetType >
template<typename... Targs>
SpinnerParent<WidgetType>::SpinnerParent(Targs... a_args)
    :
      WidgetType(a_args...)
{
	// static_assert( ::std::is_base_of<Node,ParentType>(), "ParentType should be child of Node" );
	m_margins = 0;
    m_shouldSpin = 0;
	TakeFrameOfWindow(&m_spinnerBox);
	if(m_spinnerBox.isVisible()){m_spinnerBox.hide();}
	m_spinnerBox.setParent(this);
	m_spinnerBox.setWindowModality(Qt::WindowModal);
	m_spinnerBox.raise();
}


template <typename WidgetType >
SpinnerParent<WidgetType>::~SpinnerParent()
{
	StopSpinning();
}


template <typename WidgetType >
void SpinnerParent<WidgetType>::SetSpinnerMargins(int a_margins)
{
	m_margins = a_margins;
}


template <typename WidgetType >
void SpinnerParent<WidgetType>::StartSpinning()
{
    m_shouldSpin = 1;
    if(WidgetType::isVisible()){
        if(!m_spinnerBox.isVisible()){
            m_spinnerBox.show();
            m_spinnerBox.StartSpinning();
            m_spinnerBox.CenterSpinner(this,m_margins);
            m_spinnerBox.raise();
        }
    }
}


template <typename WidgetType >
void SpinnerParent<WidgetType>::StopSpinning()
{
    m_shouldSpin = 0;
	if(m_spinnerBox.isVisible()){
		m_spinnerBox.StopSpinning();
		m_spinnerBox.hide();
	}
}


template <typename WidgetType >
void SpinnerParent<WidgetType>::resizeEvent(QResizeEvent* a_event)
{
	WidgetType::resizeEvent(a_event);
	
	if(m_spinnerBox.isVisible()){
		m_spinnerBox.CenterSpinner(this,m_margins);
		m_spinnerBox.raise();
	}
}


template <typename WidgetType >
void SpinnerParent<WidgetType>::moveEvent(QMoveEvent* a_event)
{
	WidgetType::moveEvent(a_event);
	
	if(m_spinnerBox.isVisible()){
		m_spinnerBox.CenterSpinner(this,m_margins);
		m_spinnerBox.raise();
	}
}


template <typename WidgetType >
void SpinnerParent<WidgetType>::hideEvent(QHideEvent* a_event)
{
    StopSpinning();
	WidgetType::hideEvent(a_event);
}


template <typename WidgetType >
void SpinnerParent<WidgetType>::showEvent(QShowEvent* a_event)
{
	WidgetType::showEvent(a_event);
    m_spinnerBox.hide();
    if(m_shouldSpin){
        StartSpinning();
    }
}



}} // namespace qtutils { namespace ui{


#endif  // #ifndef QTUTILS_INCLUDE_QTUTILS_UI_SPINNERPARENT_IMPL_HPP
