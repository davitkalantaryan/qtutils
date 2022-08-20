//
// file:            focust_ui_passwordbox.hpp
// path:			src/ui/focust_ui_passwordbox.hpp
// created on:		2022 Jan 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/ui/passwordbox.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QLabel>
#include <QResizeEvent>
#include <QMouseEvent>


namespace qtutils { namespace ui{


class CPPUTILS_DLL_PRIVATE PswWndLabel final : public Label01
{
public:
    PswWndLabel(PasswordBox* a_parent);

private:
    inline void EyeIconClicked();

private:
    bool        m_isPswVisible;
};


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

PasswordBox::~PasswordBox()
{
}


PswWndLabel* PasswordBox::InitLabel(PasswordBox* a_pThis)
{
    return new PswWndLabel(a_pThis);
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

void PswWndLabel::EyeIconClicked()
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

PswWndLabel::PswWndLabel(PasswordBox* a_parent)
    :
      Label01([](Label01* a_pLbl){static_cast<PswWndLabel*>(a_pLbl)->EyeIconClicked();},a_parent)
{
    SetProperParamsToImagedLbl(this);
    m_isPswVisible = true;
    EyeIconClicked();
}


}}  // namespace qtutils { namespace ui{
