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
#include <qtutils/disable_utils_warnings.h>
#include <QLineEdit>


namespace qtutils { namespace ui{

class CPPUTILS_DLL_PRIVATE PasswordBox_p;

class PasswordBox QTUTILS_EXPORT : public QLineEdit
{
public:
    template<typename... Targs>
    PasswordBox(Targs... a_args);
    virtual ~PasswordBox() override;
    
protected:
    void resizeEvent(QResizeEvent *event) override;
    
private:
    static PasswordBox_p* CreatePasswordBox_p(PasswordBox*);
    
private:
    PasswordBox_p*const m_pass_data_p;
};


}}  // namespace qtutils { namespace ui{


#ifndef QTUTILS_INCLUDE_QTUTILS_UI_PASSWORDBOX_IMPL_HPP
#include "passwordbox.impl.hpp"
#endif



#endif  // #ifndef QTUTILS_INCLUDE_QTUTILS_UI_PASSWORDBOX_HPP
