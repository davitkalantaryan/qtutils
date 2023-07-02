//
// repo:            qtutils
// file:            qtutils_ui_widgetscontainer.cpp
// path:			src/ui/qtutils_ui_widgetscontainer.cpp
// created on:		2023 Jul 02
// created by:		Davit Kalantaryan (davit@focust.ai)
//

#include <qtutils/ui/widgetscontainer.hpp>
#include <qtutils/ui/applicationwithlogin.hpp>
#include <qtutils/core/settings.hpp>
#include <qtutils/core/logger.hpp>
#include <qtutils/ui/global_functions.hpp>
#include <cinternal/flagshelper02.h>
#include <qtutils/disable_utils_warnings.h>
#include <QMessageBox>


namespace qtutils { namespace ui{


WCMainWindow::~WCMainWindow()
{
}


WCMainWindow::WCMainWindow(WidgetsContainer* a_widgsContainer)
    :
      m_pWgtsContainer(a_widgsContainer)
{
}


void WCMainWindow::InitAndShow()
{
    show();
}


#define MONITOR_TEST_SHOW_MAIN_WND_KEY  "qtutils/widgets_container/mainWnd/isVisible"


WidgetsContainer::~WidgetsContainer()
{
}


WidgetsContainer::WidgetsContainer(WCMainWindow* a_pMainWindow)
    :
      m_pMainWnd(a_pMainWindow),
      m_actionShow(QIcon(":/qtutils/show_password.png"),"&Show"),
      m_actionHide(QIcon(":/qtutils/hide_password.png"),"&Hide"),
      m_pActionShowOrHide(&m_actionShow)
{
    ApplicationWithLogin*const pThisApp = qtutilsUiAppWithLogin();
    
    m_flags.wr_all = CPPUTILS_INIT_BITS;    
    m_flags.wr.displayOrNotMainWindows = CPPUTILS_MAKE_BITS_TRUE;
        
#ifdef FOCUST_P01_SYSTRY_NEEDED

    m_tryIcon.setIcon(QIcon(":/focust/focust_logo.png"));
    m_tryIcon.setToolTip("FocusTeam application " __DATE__ " " __TIME__ );
    
    m_contextMenu.addAction( QIcon(":/focust/exit_app.png"),"Exit",pThisApp,[](){
        focustP01AppBase()->ExitApp();
    });
    
    m_tryIcon.setContextMenu(&m_contextMenu);

#endif  //  #ifdef FOCUST_P01_SYSTRY_NEEDED

    ConnectSignals();
	
	if(pThisApp->isLoggedIn()){
        const Settings aSettings(pThisApp->username());
        m_flags.wr.displayOrNotMainWindows = aSettings.value(MONITOR_TEST_SHOW_MAIN_WND_KEY,true).toBool()?
                    CPPUTILS_MAKE_BITS_TRUE:CPPUTILS_MAKE_BITS_FALSE;
		if(m_flags.rd.displayOrNotMainWindows_true){            
#ifdef FOCUST_P01_SYSTRY_NEEDED
            m_contextMenu.addAction(&m_actionHide);
#endif
            m_pActionShowOrHide = &m_actionHide;
		}
		else{            
#ifdef FOCUST_P01_SYSTRY_NEEDED
            m_contextMenu.addAction(&m_actionShow);
#endif
            m_pActionShowOrHide = &m_actionShow;
		}
	}	

}


void WidgetsContainerBase::ConnectSignals()
{
    ApplicationBase*const pThisApp = focustP01AppBase();

    //

    connect(&m_actionShow,&QAction::triggered,this,[this](){
        SwitchMainWindowToShow();
    });

    connect(&m_actionHide,&QAction::triggered,this,[this](){
        SwitchMainWindowToHidden();
    });

#ifdef FOCUST_P01_SYSTRY_NEEDED
    connect(&m_tryIcon,&QSystemTrayIcon::activated,&m_mainWnd,[this](QSystemTrayIcon::ActivationReason a_reason){
        switch(a_reason){
        case QSystemTrayIcon::Context:{
            //QMenu aMenu;
            //aMenu.addAction( QIcon(":/focust/exit_app.png"),"Exit",monitorApp(),[](){
            //    QCoreApplication::quit();
            //});
            //aMenu.exec();
        }break;
        case QSystemTrayIcon::DoubleClick:{
            Application* pThisApp = monitorApp();
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
#endif  //  #ifdef FOCUST_P01_SYSTRY_NEEDED

    connect(m_pMainWnd,&MainWindowBase::ClosedSignal,m_pMainWnd,[this](){
        SwitchMainWindowToHidden();
    });


    //

    QObject::connect(pThisApp,&ApplicationBase::aboutToQuit,pThisApp,[this](){
#ifdef FOCUST_P01_SYSTRY_NEEDED
        m_contextMenu.hide();
        m_tryIcon.hide();
#endif
        m_pMainWnd->hide();
        m_loginWnd.hide();
    });


    QObject::connect(pThisApp,&ApplicationBase::AppGoingToExitSignal,pThisApp,[this](){
#ifdef FOCUST_P01_SYSTRY_NEEDED
        m_contextMenu.hide();
        m_tryIcon.hide();
#endif
        m_pMainWnd->hide();
        m_loginWnd.hide();
    });

    QObject::connect(pThisApp,&ApplicationBase::LoginSucceedSignal,m_pMainWnd,[this](){
        ApplicationBase*const pThisApp = focustP01AppBase();
        const Settings aSettings(pThisApp->username());
        m_flags.wr.displayOrNotMainWindows = aSettings.value(MONITOR_TEST_SHOW_MAIN_WND_KEY,true).toBool()?
                    CPPUTILS_MAKE_BITS_TRUE:CPPUTILS_MAKE_BITS_FALSE;

        if(m_loginWnd.isVisible()){m_loginWnd.hide();}
        if(m_pMainWnd->isHidden()){
            if(m_flags.rd.displayOrNotMainWindows_true){
                SwitchMainWindowToShow();
            }
            else{
                SwitchMainWindowToHidden();
            }
        }
    });

    QObject::connect(pThisApp,&ApplicationBase::LoggedOutSignal,m_pMainWnd,[this](bool a_isAppExit){
        if(m_pMainWnd->isVisible()){m_pMainWnd->hide();}
        if(a_isAppExit){
            if(m_loginWnd.isVisible()){m_loginWnd.hide();}
        }
        else{
            if(m_loginWnd.isHidden()){m_loginWnd.InitAndShow();}
        }        
#ifdef FOCUST_P01_SYSTRY_NEEDED
        m_contextMenu.removeAction(m_pActionShowOrHide);
#endif
    });


    QObject::connect(pThisApp,&ApplicationBase::LoginFailedSignal,m_pMainWnd,[](QString a_errorDetails){
        const QString title = "Login failed";
        const QString text = a_errorDetails;
        ::qtutils::ui::ShowMessageBox(QMessageBox::Critical,title,text);
    });
}


void WidgetsContainerBase::show()
{
    ApplicationBase*const pThisApp = focustP01AppBase();

#ifdef FOCUST_P01_SYSTRY_NEEDED
    m_tryIcon.show();
#endif
	
	if(pThisApp->isLoggedIn()){
		m_loginWnd.hide();
		if(m_flags.rd.displayOrNotMainWindows_true){            
            m_pMainWnd->InitAndShow();
		}
	}
	else{
        m_pMainWnd->hide();
        m_loginWnd.InitAndShow();
	}
	
}


void WidgetsContainerBase::SwitchMainWindowToHidden()
{
    ApplicationBase*const pThisApp = focustP01AppBase();
    Settings aSettings(pThisApp->username());
	aSettings.setValue(MONITOR_TEST_SHOW_MAIN_WND_KEY,false);
	m_flags.wr.displayOrNotMainWindows = CPPUTILS_MAKE_BITS_FALSE;    
#ifdef FOCUST_P01_SYSTRY_NEEDED
    m_contextMenu.removeAction(m_pActionShowOrHide);
    m_contextMenu.addAction(&m_actionShow);
#endif
    m_pActionShowOrHide = &m_actionShow;
    m_pMainWnd->hide();
}


void WidgetsContainerBase::SwitchMainWindowToShow()
{
    ApplicationBase*const pThisApp = focustP01AppBase();
    Settings aSettings(pThisApp->username());
	aSettings.setValue(MONITOR_TEST_SHOW_MAIN_WND_KEY,true);
	m_flags.wr.displayOrNotMainWindows = CPPUTILS_MAKE_BITS_TRUE;    
#ifdef FOCUST_P01_SYSTRY_NEEDED
    m_contextMenu.removeAction(m_pActionShowOrHide);
    m_contextMenu.addAction(&m_actionHide);
#endif
    m_pActionShowOrHide = &m_actionHide;    
    m_pMainWnd->InitAndShow();
}


}}}  //  namespace focust { namespace p01 { namespace ui{
