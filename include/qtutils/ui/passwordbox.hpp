//
// file:            passwordbox.hpp
// path:			include/focust/ui/passwordbox.hpp
// created on:		2022 Jan 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef FOCUST_INCLUDE_PASSWORDBOX_HPP
#define FOCUST_INCLUDE_PASSWORDBOX_HPP


#include <focust/core/focust_internal_header.h>
#include <focust/core/disable_utils_warnings.h>
#include <QLineEdit>


namespace focust { namespace ui{

class FOCUST_DLL_PRIVATE PasswordBox_p;

class PasswordBox final
{
public:
    PasswordBox(QWidget* parent=nullptr);
    ~PasswordBox();
    PasswordBox(const PasswordBox&)=delete;
    PasswordBox(PasswordBox&&)=delete;
    PasswordBox& operator=(const PasswordBox&)=delete;
    PasswordBox& operator=(PasswordBox&&)=delete;
    QLineEdit* lineEdit()const;
private:
    PasswordBox_p*const m_pass_data_p;
};


}}  // namespace focust { namespace test{





#endif  // #ifndef FOCUST_INCLUDE_PASSWORDBOX_HPP
