//
// file:            passwordbox02.hpp
// path:			include/qtutils/ui/passwordbox02.hpp
// created on:		2022 Aug 20
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_PASSWORDBOX02_HPP
#define QTUTILS_INCLUDE_QTUTILS_UI_PASSWORDBOX02_HPP


#include <qtutils/qtutils_internal_header.h>
#include <qtutils/ui/wgtwithlblinright.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QLineEdit>


namespace qtutils { namespace ui{


class CPPUTILS_DLL_PRIVATE PswWndLabel02;

class PasswordBox02 QTUTILS_EXPORT : public WgtWithLblInRight<QLineEdit>
{
public:
    template<typename... Targs>
    PasswordBox02(Targs... a_args);
    virtual ~PasswordBox02() override;

private:
    void SetLabelInRight(QLabel* a_pLabelInRight)=delete;
        
private:
    void InitLabel();
    
};


}}  // namespace qtutils { namespace ui{


#ifndef QTUTILS_INCLUDE_QTUTILS_UI_PASSWORDBOX02_IMPL_HPP
#include "passwordbox02.impl.hpp"
#endif



#endif  // #ifndef QTUTILS_INCLUDE_QTUTILS_UI_PASSWORDBOX02_HPP
