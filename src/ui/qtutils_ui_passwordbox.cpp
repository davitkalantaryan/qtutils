//
// file:            focust_ui_passwordbox.hpp
// path:			src/ui/focust_ui_passwordbox.hpp
// created on:		2022 Jan 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/ui/passwordbox.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QLabel>
#include <QMouseEvent>


namespace qtutils { namespace ui{


class CPPUTILS_DLL_PRIVATE PswWndLabel final : public QLabel
{
public:
    PswWndLabel(PasswordBox* a_parent);

private:
    void mousePressEvent(QMouseEvent *event) override;
    inline void EyeIconClicked();

private:
    bool        m_isPswVisible;
};


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/


PasswordBox::~PasswordBox()
{
}


void PasswordBox::Init()
{
    setEchoMode(QLineEdit::Password);
    WgtWithLblInRight<QLineEdit>::SetLabelInRight(new PswWndLabel(this));
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

PswWndLabel::PswWndLabel(PasswordBox* a_parent)
    :
      QLabel(a_parent)
{
    SetProperParamsToImagedLbl(this);
    m_isPswVisible = (a_parent->echoMode()==QLineEdit::Password)?true:false;
    EyeIconClicked();
}


void PswWndLabel::mousePressEvent(QMouseEvent* a_event)
{
    QLabel::mousePressEvent(a_event);
    switch(a_event->button()){
    case Qt::LeftButton:
        EyeIconClicked();
        break;
    default:
        break;
    }
}


void PswWndLabel::EyeIconClicked()
{
    PasswordBox* pParent = static_cast<PasswordBox*>(parentWidget());
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


}}  // namespace qtutils { namespace ui{
