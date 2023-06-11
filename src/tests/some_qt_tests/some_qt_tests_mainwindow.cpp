//
// file:		some_qt_tests_mainwindow.cpp
// path:		src/tests/some_qt_tests/some_qt_tests_mainwindow.cpp
// created on:	2023 Jun 10
// creatd by:	Davit Kalantaryan (davit.kalantaryan@desy.de)
//


#include "some_qt_tests_mainwindow.hpp"
#include <qtutils/core/settings.hpp>
#include <qtutils/ui/global_functions.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QMessageBox>
#include <QCoreApplication>
#include <QLabel>


namespace qtutils { namespace some_qt_tests{


MainWindowRaw::MainWindowRaw(const QString& a_initValue)
{
    
    m_showMsgBoxBtn.setText("Show Msg. Box");
    m_closeBtn.setText("Exit");
    m_anyTextEdit.setText(a_initValue);
    
    m_mainLayout.addWidget(new QLabel(SOME_QT_TEST_VERSION),0,0,1,2);
    m_mainLayout.addWidget(&m_anyTextEdit,1,0,1,2);
    m_mainLayout.addWidget(&m_showMsgBoxBtn,2,0,1,1);
    m_mainLayout.addWidget(&m_closeBtn,2,1,1,1);
    
    setLayout(&m_mainLayout);
    
    connect(&m_showMsgBoxBtn,&QPushButton::clicked,this,[](){
        ui::ShowMessageBox(QMessageBox::Information,"MsgTitle","MsgText");
    });
    
    connect(&m_closeBtn,&QPushButton::clicked,this,[this](){
        close();
        QCoreApplication::quit();
    });
    
    connect(&m_anyTextEdit,&QLineEdit::textEdited,this,[](const QString& a_newText){
        ::qtutils::Settings aSettings;
        aSettings.setValue(SOME_QT_TEST_KEY01,a_newText);
    });
}


}}  // namespace qtutils { namespace some_qt_tests{
