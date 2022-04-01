//
// file:            main_loader01_test.cpp
// path:			src/tests/main_loader01_test.cpp
// created on:		2022 Apr 01
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/ui/spinnerparent.hpp>
#include <qtutils/ui/passwordbox.hpp>
#include <qtutils/ui/logintypewindow.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QApplication>
#include <QGridLayout>
#include <QPushButton>
#include <QTimer>

class LoginExmWnd : public ::qtutils::ui::LoginTypeWindow< ::qtutils::ui::SpinnerParent<QWidget> >
{
public:
	LoginExmWnd();
	
private:
	QGridLayout         m_mainLayout;
    QLineEdit           m_userUidEdit;
    ::qtutils::ui::PasswordBox     m_passwordEdit;
	QPushButton         m_settingsButton;
    QPushButton         m_loginStartButton;
    QPushButton         m_exitButton;
};


int main(int a_argc, char* a_argv[])
{
	QApplication aApp(a_argc,a_argv);
	
	
	//qtutils::ui::SpinnerParent<QWidget> aSpinBox;
	LoginExmWnd aSpinBox;
	aSpinBox.SetSpinnerMargins(5);
	aSpinBox.InitAndShow();
	aSpinBox.StartSpinning();
	
	QTimer::singleShot(5000,&aSpinBox,[&aSpinBox](){
		aSpinBox.StopSpinning();
	});
	
	aApp.exec();
	aSpinBox.StopSpinning();
	
	
	//QStackedLayout aStL;
	//qtutils::ui::SpinnerBox spB;
	//QWidget aWidget;
	//aStL.addWidget(&aWidget);
	//aStL.addWidget(&spB);
	//aStL.setCurrentIndex(0);
	//aStL.setStackingMode(QStackedLayout::StackAll);
	//aApp.exec();
	
	
	return 0;
}


LoginExmWnd::LoginExmWnd()
{
	m_settingsButton.setText("SETTNGS");
	m_loginStartButton.setText("LOGIN");
    m_exitButton.setText("EXIT");
    m_exitButton.setIcon(QIcon(":/img/exit_app.png"));

    m_userUidEdit.setPlaceholderText("Email/Username");
    m_passwordEdit.setPlaceholderText("Password");

	m_settingsButton.setStyleSheet("background-color:rgb(0,0,255);" "color:white;");
    m_loginStartButton.setStyleSheet("background-color:rgb(0,0,255);" "color:white;");
    m_exitButton.setStyleSheet("background-color:rgb(0,0,255);" "color:white;");
    
	setStyleSheet("background-color:rgb(220,220,220);");
	int nRowNumber = -1;

	m_mainLayout.addWidget(new QLabel("Login Window"),++nRowNumber,0,1,2);
	
	m_mainLayout.addWidget(new QLabel("Email(or Uid):"),++nRowNumber,0,1,1);
    m_mainLayout.addWidget(&m_userUidEdit,nRowNumber,1,1,3);
		
	m_mainLayout.addWidget(new QLabel("Password:"),++nRowNumber,0,1,1);
    m_mainLayout.addWidget(&m_passwordEdit,nRowNumber,1,1,3);
	
    m_mainLayout.addWidget(&m_exitButton,++nRowNumber,1,1,1);
    m_mainLayout.addWidget(&m_settingsButton,nRowNumber,2,1,1);
	m_mainLayout.addWidget(&m_loginStartButton,nRowNumber,3,1,1);

	QLabel* pIconLbl = new QLabel();
    pIconLbl->setPixmap( QPixmap( ":/img/focust_logo.png" ) );
	pIconLbl->setScaledContents( true );
	pIconLbl->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
    m_mainLayout.addWidget(new QLabel(QString("Copyright ")+QChar(169)+QString(" 2021 FocusT, All rights reserved")),6,0,1,3);
	m_mainLayout.addWidget(pIconLbl,6,3,1,1);

    setLayout(&m_mainLayout);
}
