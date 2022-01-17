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
};


class CPPUTILS_DLL_PRIVATE PasswordBox_p final : public QLineEdit
{
public:
    PasswordBox_p(QWidget* parent);

    void EyeIconClicked();

private:
    void resizeEvent(QResizeEvent *event) override;

public:
    PswWndLabel  eyeLabel;
    bool        isPswVisible;
};


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

PasswordBox::PasswordBox(QWidget* a_parent)
    :
      m_pass_data_p(new PasswordBox_p(a_parent))
{
}


PasswordBox::~PasswordBox()
{
    delete m_pass_data_p;
}


QLineEdit* PasswordBox::lineEdit()const
{
    return m_pass_data_p;
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

PasswordBox_p::PasswordBox_p(QWidget* a_parent)
    :
      QLineEdit(a_parent),
      eyeLabel(this),
      isPswVisible(true)
{
    this->eyeLabel.setScaledContents(true);
    EyeIconClicked();
}


void PasswordBox_p::resizeEvent(QResizeEvent* a_event)
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
    this->eyeLabel.resize(eyeSizeLn,eyeSizeLn);
    this->eyeLabel.move(newSize.width()-eyeSizeLn-1,1);
}


void PasswordBox_p::EyeIconClicked()
{
    if(this->isPswVisible){
        this->eyeLabel.setToolTip("Show password");
        this->eyeLabel.setPixmap( QPixmap( ":/img/show_password.png" ) );
        setEchoMode(QLineEdit::Password);
        this->isPswVisible = false;
    }
    else{
        this->eyeLabel.setToolTip("Hide password");
        this->eyeLabel.setPixmap( QPixmap( ":/img/hide_password.png" ) );
        setEchoMode(QLineEdit::Normal);
        this->isPswVisible = true;
    }
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

PswWndLabel::PswWndLabel(PasswordBox_p* a_parent)
    :
      QLabel(a_parent)
{
}


void PswWndLabel::mousePressEvent(QMouseEvent* a_event)
{
    QLabel::mousePressEvent(a_event);
    switch(a_event->button()){
    case Qt::LeftButton:
        static_cast<PasswordBox_p*>(parent())->EyeIconClicked();
        break;
    default:
        break;
    }
}


}}  // namespace qtutils { namespace ui{
