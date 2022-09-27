//
// file:            wgtwithwgtinleft.hpp
// path:			include/qtutils/ui/wgtwithwgtinleft.hpp
// created on:		2022 Sep 26
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_WGTWITHWGTINLEFT_HPP
#define QTUTILS_INCLUDE_QTUTILS_UI_WGTWITHWGTINLEFT_HPP


#include <qtutils/export_symbols.h>
#include <qtutils/ui/sizeapplywindow.hpp>
#include <functional>
#include <qtutils/disable_utils_warnings.h>


namespace qtutils { namespace ui{


template <typename WidgetType, typename LeftWgtType>
class QTUTILS_EXPORT WgtWithWgtInLeft : public ::qtutils::ui::SizeApplyWindow<WidgetType>
{
public:
    template<typename... Targs>
    WgtWithWgtInLeft(Targs... a_args);
    virtual ~WgtWithWgtInLeft() override;

    LeftWgtType* wgtInLeft()const;
    void    SetWgtInLeft(LeftWgtType* a_pWgtInLeft);

protected:
    virtual void ApplyNewSize(const QSize& a_newSize) override;

protected:
    LeftWgtType*     m_pWgtInLeft;
};


}}  // namespace qtutils { namespace ui{


#ifndef QTUTILS_INCLUDE_QTUTILS_UI_WGTWITHWGTINLEFT_IMPL_HPP
#include "wgtwithwgtinleft.impl.hpp"
#endif



#endif  // #ifndef QTUTILS_INCLUDE_QTUTILS_UI_WGTWITHWGTINLEFT_HPP
