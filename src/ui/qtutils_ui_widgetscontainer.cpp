//
// repo:            qtutils
// file:            qtutils_ui_widgetscontainer.cpp
// path:			src/ui/qtutils_ui_widgetscontainer.cpp
// created on:		2023 Jul 02
// created by:		Davit Kalantaryan (davit@focust.ai)
//

#include <qtutils/ui/widgetscontainer.hpp>
#include <qtutils/ui/applicationwithloginqu.hpp>
#include <qtutils/core/settings.hpp>
#include <qtutils/core/logger.hpp>
#include <qtutils/ui/global_functions.hpp>
#include <cinternal/flagshelper02.h>
#include <qtutils/disable_utils_warnings.h>
#include <QMessageBox>


namespace qtutils { namespace ui{


#define MONITOR_TEST_SHOW_MAIN_WND_KEY  "qtutils/widgets_container/mainWnd/isVisible"


WidgetsContainer::~WidgetsContainer()
{
}


WidgetsContainer::WidgetsContainer(QWidget* CPPUTILS_ARG_NN a_pLoginWnd, QWidget* CPPUTILS_ARG_NN a_pMainWindow,
                                   const QString& a_logo, const QString& a_tooltip, const QString& a_exitLogo,
                                   const TypeWCShow& a_login_show, const TypeWCShow& a_main_show)
    :
      m_pLoginWnd(a_pLoginWnd),
      m_pMainWnd(a_pMainWindow),
      m_login_show(a_login_show),
      m_main_show(a_main_show),
      m_actionShow(QIcon(":/qtutils/show_password.png"),"&Show"),
      m_actionHide(QIcon(":/qtutils/hide_password.png"),"&Hide"),
      m_pActionShowOrHide(&m_actionShow)
{
    ApplicationWithLogin*const pThisApp = qtutilsUiAppWithLogin();
    
    m_flags.wr_all = CPPUTILS_INIT_BITS;    
    m_flags.wr.displayOrNotMainWindows = CPPUTILS_MAKE_BITS_TRUE;
        
#ifdef QTUTILS_UI_WC_SYSTRY_NEEDED

    //m_tryIcon.setIcon(QIcon(":/focust/focust_logo.png"));
    //m_tryIcon.setToolTip("FocusTeam application " __DATE__ " " __TIME__ );
    m_tryIcon.setIcon(QIcon(a_logo));
    m_tryIcon.setToolTip(a_tooltip);
    
    //m_contextMenu.addAction( QIcon(":/focust/exit_app.png"),"Exit",pThisApp,[](){
    m_contextMenu.addAction( QIcon(a_exitLogo),"Exit",pThisApp,[](){
        qtutilsUiAppWithLogin()->ExitApp();
    });
    
    m_tryIcon.setContextMenu(&m_contextMenu);

#endif  //  #ifdef QTUTILS_UI_WC_SYSTRY_NEEDED

    ConnectSignals();
	
	if(pThisApp->isLoggedIn()){
        const Settings aSettings;
        m_flags.wr.displayOrNotMainWindows = aSettings.value(MONITOR_TEST_SHOW_MAIN_WND_KEY,true).toBool()?
                    CPPUTILS_BISTATE_MAKE_BITS_TRUE:CPPUTILS_BISTATE_MAKE_BITS_FALSE;
		if(m_flags.rd.displayOrNotMainWindows_true){            
#ifdef QTUTILS_UI_WC_SYSTRY_NEEDED
            m_contextMenu.addAction(&m_actionHide);
#endif
            m_pActionShowOrHide = &m_actionHide;
		}
		else{            
#ifdef QTUTILS_UI_WC_SYSTRY_NEEDED
            m_contextMenu.addAction(&m_actionShow);
#endif
            m_pActionShowOrHide = &m_actionShow;
		}
	}	

}


void WidgetsContainer::ConnectSignals()
{
    ApplicationWithLogin*const pThisApp = qtutilsUiAppWithLogin();

    connect(&m_actionShow,&QAction::triggered,this,[this](){
        SwitchMainWindowToShow();
    });

    connect(&m_actionHide,&QAction::triggered,this,[this](){
        SwitchMainWindowToHidden();
    });

#ifdef QTUTILS_UI_WC_SYSTRY_NEEDED
    connect(&m_tryIcon,&QSystemTrayIcon::activated,m_pMainWnd,[this](QSystemTrayIcon::ActivationReason a_reason){
        switch(a_reason){
        case QSystemTrayIcon::Context:{
            //QMenu aMenu;
            //aMenu.addAction( QIcon(":/focust/exit_app.png"),"Exit",monitorApp(),[](){
            //    QCoreApplication::quit();
            //});
            //aMenu.exec();
        }break;
        case QSystemTrayIcon::DoubleClick:{
            ApplicationWithLogin*const pThisApp = qtutilsUiAppWithLogin();
            if(pThisApp->isLoggedIn()){
                if(m_flags.rd.displayOrNotMainWindows_false){
                    SwitchMainWindowToShow();
                }
            }
        }break;
        default:
            break;
        }
    });
#endif  //  #ifdef QTUTILS_UI_WC_SYSTRY_NEEDED

    QObject::connect(pThisApp,&ApplicationWithLogin::aboutToQuit,pThisApp,[this](){
#ifdef QTUTILS_UI_WC_SYSTRY_NEEDED
        m_contextMenu.hide();
        m_tryIcon.hide();
#endif
        m_pMainWnd->hide();
        m_pLoginWnd->hide();
    });


    QObject::connect(pThisApp,&ApplicationWithLogin::AppGoingToExitSignal,pThisApp,[this](){
#ifdef QTUTILS_UI_WC_SYSTRY_NEEDED
        m_contextMenu.hide();
        m_tryIcon.hide();
#endif
        m_pMainWnd->hide();
        m_pLoginWnd->hide();
    });

    QObject::connect(pThisApp,&ApplicationWithLogin::LoginSucceedSignal,m_pMainWnd,[this](){
        const Settings aSettings;
        m_flags.wr.displayOrNotMainWindows = aSettings.value(MONITOR_TEST_SHOW_MAIN_WND_KEY,true).toBool()?
                    CPPUTILS_MAKE_BITS_TRUE:CPPUTILS_MAKE_BITS_FALSE;

        if(m_pLoginWnd->isVisible()){m_pLoginWnd->hide();}
        if(m_pMainWnd->isHidden()){
            if(m_flags.rd.displayOrNotMainWindows_true){
                SwitchMainWindowToShow();
            }
            else{
                SwitchMainWindowToHidden();
            }
        }
    });

    QObject::connect(pThisApp,&ApplicationWithLogin::LoggedOutSignal,m_pMainWnd,[this](bool a_isAppExit){
        if(m_pMainWnd->isVisible()){m_pMainWnd->hide();}
        if(a_isAppExit){
            if(m_pLoginWnd->isVisible()){m_pLoginWnd->hide();}
        }
        else{
            if(m_pLoginWnd->isHidden()){m_login_show(m_pLoginWnd);}
        }        
#ifdef QTUTILS_UI_WC_SYSTRY_NEEDED
        m_contextMenu.removeAction(m_pActionShowOrHide);
#endif
    });


    QObject::connect(pThisApp,&ApplicationWithLogin::LoginFailedSignal,m_pMainWnd,[](QString a_errorDetails){
        const QString title = "Login failed";
        const QString text = a_errorDetails;
        ::qtutils::ui::ShowMessageBox(QMessageBox::Critical,title,text);
    });
}


void WidgetsContainer::show()
{
    ApplicationWithLogin*const pThisApp = qtutilsUiAppWithLogin();

#ifdef QTUTILS_UI_WC_SYSTRY_NEEDED
    m_tryIcon.show();
#endif
	
	if(pThisApp->isLoggedIn()){
        m_pLoginWnd->hide();
		if(m_flags.rd.displayOrNotMainWindows_true){            
            m_main_show(m_pMainWnd);
		}
	}
	else{
        m_pMainWnd->hide();
        m_login_show(m_pLoginWnd);
	}	
}


void WidgetsContainer::SwitchMainWindowToHidden()
{
    Settings aSettings;
	aSettings.setValue(MONITOR_TEST_SHOW_MAIN_WND_KEY,false);
	m_flags.wr.displayOrNotMainWindows = CPPUTILS_MAKE_BITS_FALSE;    
#ifdef QTUTILS_UI_WC_SYSTRY_NEEDED
    m_contextMenu.removeAction(m_pActionShowOrHide);
    m_contextMenu.addAction(&m_actionShow);
#endif
    m_pActionShowOrHide = &m_actionShow;
    m_pMainWnd->hide();
}


void WidgetsContainer::SwitchMainWindowToShow()
{
    Settings aSettings;
	aSettings.setValue(MONITOR_TEST_SHOW_MAIN_WND_KEY,true);
	m_flags.wr.displayOrNotMainWindows = CPPUTILS_MAKE_BITS_TRUE;    
#ifdef QTUTILS_UI_WC_SYSTRY_NEEDED
    m_contextMenu.removeAction(m_pActionShowOrHide);
    m_contextMenu.addAction(&m_actionHide);
#endif
    m_pActionShowOrHide = &m_actionHide;    
    m_main_show(m_pMainWnd);
}


}}  //  namespace qtutils { namespace ui{
