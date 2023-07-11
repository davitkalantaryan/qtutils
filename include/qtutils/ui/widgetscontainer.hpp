//
// repo:            qtutils
// file:            widgetscontainer.hpp
// path:			include/qtutils/ui/widgetscontainer.hpp
// created on:		2023 Jul 02
// created by:		Davit Kalantaryan (davit@focust.ai)
//

#pragma once

#include <qtutils/export_symbols.h>
#include <cpputils/functional.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QAction>
#ifdef QTUTILS_UI_WC_SYSTRY_NEEDED
#include <QSystemTrayIcon>
#include <QMenu>
#endif

namespace qtutils { namespace ui{


class QTUTILS_UI_EXPORT WidgetsContainer : public QObject
{
public:
    typedef ::std::function<void(QWidget*)> TypeWCShow;

public:
    virtual ~WidgetsContainer() override;
    WidgetsContainer(QWidget* CPPUTILS_ARG_NN a_pLoginWnd, QWidget* CPPUTILS_ARG_NN a_pMainWindow,
                     const QString& a_logo, const QString& a_tooltip,const QString& a_exitLogo,
                     const TypeWCShow& a_login_show, const TypeWCShow& a_main_show);
    
public slots:
    void show();
    void hide();
    void SwitchMainWindowToHidden();

private:
	void SwitchMainWindowToShow();
    void ConnectSignals();

protected:
    QWidget*const           m_pLoginWnd;
    QWidget*const           m_pMainWnd;
    const TypeWCShow        m_login_show;
    const TypeWCShow        m_main_show;
#ifdef QTUTILS_UI_WC_SYSTRY_NEEDED
    QSystemTrayIcon         m_tryIcon;
    QMenu                   m_contextMenu;
#endif
    QAction                 m_actionShow;
    QAction                 m_actionHide;
    QAction*                m_pActionShowOrHide;
    
    union{
		uint64_t wr_all;
		struct{
			uint64_t	displayOrNotMainWindows_true : 1;
			uint64_t	displayOrNotMainWindows_false : 1;
			uint64_t	reserved64bit : 62;
		}rd;
		struct{
			uint64_t	displayOrNotMainWindows : 2;
			uint64_t	reserved64bit : 62;
		}wr;
	}m_flags;
};


}}  //  namespace qtutils { namespace ui{
