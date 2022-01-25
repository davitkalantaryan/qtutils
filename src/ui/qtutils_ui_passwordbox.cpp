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


class CPPUTILS_DLL_PRIVATE PswWndLabel final : public QLabel
{
public:
    PswWndLabel(PasswordBox_p* parent);

private:
    void mousePressEvent(QMouseEvent *event) override;
private:
    PasswordBox_p*const m_pParent;
};


class CPPUTILS_DLL_PRIVATE PasswordBox_p final
{
public:
    PasswordBox_p(PasswordBox* a_pParent);

    void EyeIconClicked();

private:
    void resizeEvent(QResizeEvent *event) ;

public:
    PasswordBox*const   m_pParent;    
    PswWndLabel         eyeLabel;
    bool                isPswVisible;
};


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/


PasswordBox::~PasswordBox()
{
    delete m_pass_data_p;
}


void PasswordBox::resizeEvent(QResizeEvent* a_event)
{
    int eyeSizeLn;
    QLineEdit::resizeEvent(a_event);
    const QSize& newSize = a_event->size();
    const int newHeight = newSize.height();
    if(newHeight>2){
        eyeSizeLn = newHeight-2;
    }
    else{
        eyeSizeLn = newHeight-2;
    }
    m_pass_data_p->eyeLabel.resize(eyeSizeLn,eyeSizeLn);
    m_pass_data_p->eyeLabel.move(newSize.width()-eyeSizeLn-1,1);
}


PasswordBox_p* PasswordBox::CreatePasswordBox_p(PasswordBox* a_pThis)
{
    return new PasswordBox_p(a_pThis);
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

PasswordBox_p::PasswordBox_p(PasswordBox* a_pParent)
    :
      m_pParent(a_pParent),
      eyeLabel(this),
      isPswVisible(true)
{
    this->eyeLabel.setScaledContents(true);
    EyeIconClicked();
}


void PasswordBox_p::EyeIconClicked()
{
    if(this->isPswVisible){
        this->eyeLabel.setToolTip("Show password");
        this->eyeLabel.setPixmap( QPixmap( ":/img/show_password.png" ) );
        m_pParent->setEchoMode(QLineEdit::Password);
        this->isPswVisible = false;
    }
    else{
        this->eyeLabel.setToolTip("Hide password");
        this->eyeLabel.setPixmap( QPixmap( ":/img/hide_password.png" ) );
        m_pParent->setEchoMode(QLineEdit::Normal);
        this->isPswVisible = true;
    }
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

PswWndLabel::PswWndLabel(PasswordBox_p* a_parent)
    :
      QLabel(a_parent->m_pParent),
      m_pParent(a_parent)
{
}


void PswWndLabel::mousePressEvent(QMouseEvent* a_event)
{
    QLabel::mousePressEvent(a_event);
    switch(a_event->button()){
    case Qt::LeftButton:
        m_pParent->EyeIconClicked();
        break;
    default:
        break;
    }
}


}}  // namespace qtutils { namespace ui{
