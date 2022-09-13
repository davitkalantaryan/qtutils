//
// file:            passwordbox.hpp
// path:			include/qtutils/ui/passwordbox.hpp
// created on:		2022 Jan 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_PASSWORDBOX_HPP
#define QTUTILS_INCLUDE_QTUTILS_UI_PASSWORDBOX_HPP


#include <qtutils/qtutils_internal_header.h>
#include <qtutils/ui/wgtwithwgtinright.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QLineEdit>
#include <QLabel>


namespace qtutils { namespace ui{


class PasswordBox QTUTILS_EXPORT : public WgtWithWgtInRight<QLineEdit,QLabel>
{
public:
    template<typename... Targs>
    PasswordBox(Targs... a_args);
    virtual ~PasswordBox() override;

private:
    void SetWgtInRight(QLabel* a_pLabelInRight) = delete;
    void Init();
};


}}  // namespace qtutils { namespace ui{


#ifndef QTUTILS_INCLUDE_QTUTILS_UI_PASSWORDBOX_IMPL_HPP
#include "passwordbox.impl.hpp"
#endif



#endif  // #ifndef QTUTILS_INCLUDE_QTUTILS_UI_PASSWORDBOX_HPP
