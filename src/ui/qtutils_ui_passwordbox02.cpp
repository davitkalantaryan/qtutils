//
// file:            focust_ui_passwordbox02.hpp
// path:			src/ui/focust_ui_passwordbox02.hpp
// created on:		2022 Aug 20
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/ui/passwordbox02.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QLabel>
#include <QResizeEvent>
#include <QMouseEvent>


namespace qtutils { namespace ui{


class CPPUTILS_DLL_PRIVATE PswWndLabel02 final : public Label01
{
public:
    PswWndLabel02(PasswordBox02* a_parent);

private:
    inline void EyeIconClicked();

private:
    bool        m_isPswVisible;
};


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

PasswordBox02::~PasswordBox02()
{
}


void PasswordBox02::InitLabel()
{
    WgtWithLblInRight<QLineEdit>::SetLabelInRight( new PswWndLabel02(this) );
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

void PswWndLabel02::EyeIconClicked()
{
    QLineEdit* pParent = static_cast<QLineEdit*>(parentWidget());

    if(m_isPswVisible){
        setToolTip("Show password");
        setPixmap( QPixmap( ":/qtutils/show_password.png" ) );
        pParent->setEchoMode(QLineEdit::Password);
        m_isPswVisible = false;
    }
    else{
        setToolTip("Hide password");
        setPixmap( QPixmap( ":/qtutils/hide_password.png" ) );
        pParent->setEchoMode(QLineEdit::Normal);
        m_isPswVisible = true;
    }
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

PswWndLabel02::PswWndLabel02(PasswordBox02* a_parent)
    :
      Label01([](Label01* a_pLbl){static_cast<PswWndLabel02*>(a_pLbl)->EyeIconClicked();},a_parent)
{
    SetProperParamsToImagedLbl(this);
    m_isPswVisible = true;
    EyeIconClicked();
}


}}  // namespace qtutils { namespace ui{
