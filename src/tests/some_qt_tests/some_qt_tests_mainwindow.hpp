//
// file:		some_qt_tests_mainwindow.hpp
// path:		src/tests/some_qt_tests/some_qt_tests_mainwindow.hpp
// created on:	2023 Jun 10
// creatd by:	Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#pragma once

#include <qtutils/ui/resiziblewindow.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QWidget>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>


#define SOME_QT_TEST_VERSION    "1.0.3"
#define SOME_QT_TEST_KEY01      "SOME_QT_TEST_KEY01"


namespace qtutils { namespace some_qt_tests{


class CPPUTILS_DLL_PRIVATE MainWindow : public QWidget
{
public:
	MainWindow(const QString& a_initValue);
		
protected:
	QGridLayout         m_mainLayout;
    QLineEdit           m_anyTextEdit;
	QPushButton         m_showMsgBoxBtn;
    QPushButton         m_closeBtn;
};



}}  //  namespace qtutils { namespace some_qt_tests{
