//
// file:            wgtwithwgtinright.hpp
// path:			include/qtutils/ui/wgtwithwgtinright.hpp
// created on:		2022 Aug 20
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_WGTWITHWGTINRIGHT_HPP
#define QTUTILS_INCLUDE_QTUTILS_UI_WGTWITHWGTINRIGHT_HPP


#include <qtutils/export_symbols.h>
#include <qtutils/ui/sizeapplywindow.hpp>
#include <functional>
#include <qtutils/disable_utils_warnings.h>


namespace qtutils { namespace ui{


template <typename WidgetType, typename RightWgtType>
class QTUTILS_EXPORT WgtWithWgtInRight : public ::qtutils::ui::SizeApplyWindow<WidgetType>
{
public:
    template<typename... Targs>
    WgtWithWgtInRight(Targs... a_args);
    virtual ~WgtWithWgtInRight() override;

    RightWgtType* wgtInRight()const;
    void    SetWgtInRight(RightWgtType* a_pLabelInRight);

protected:
    virtual void ApplyNewSize(const QSize& a_newSize) override;

protected:
    RightWgtType*     m_pWgtInRight;
};


}}  // namespace qtutils { namespace ui{


#ifndef QTUTILS_INCLUDE_QTUTILS_UI_WGTWITHWGTINRIGHT_IMPL_HPP
#include "wgtwithwgtinright.impl.hpp"
#endif



#endif  // #ifndef QTUTILS_INCLUDE_QTUTILS_UI_WGTWITHWGTINRIGHT_HPP
