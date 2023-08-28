//
// repo:            qtutils
// file:            qtutils_ui_widgetscontainerlogin.cpp
// path:			src/ui/qtutils_ui_widgetscontainerlogin.cpp
// created on:		2023 Jul 02
// created by:		Davit Kalantaryan (davit@focust.ai)
//

#include <qtutils/ui/widgetscontainerlogin.hpp>
#include <qtutils/ui/applicationwithloginqu.hpp>
#include <qtutils/core/logger.hpp>
#include <qtutils/ui/global_functions.hpp>
#include <cinternal/flagshelper02.h>
#include <qtutils/disable_utils_warnings.h>
#include <QMessageBox>


namespace qtutils { namespace ui{


static void AppExiterForRestApps(void){
    qtutilsUiAppWithLogin()->ExitApp("AppExiterForRestApps");
}


WidgetsContainerLogin::~WidgetsContainerLogin()
{
}


WidgetsContainerLogin::WidgetsContainerLogin(QWidget* CPPUTILS_ARG_NN a_pLoginWnd, QWidget* CPPUTILS_ARG_NN a_pMainWindow,
                                             const QString& a_logo, const QString& a_tooltip,const QString& a_exitLogo,
                                             const TypeWCShowHide& a_login_show, const TypeWCShowHide& a_main_show)
    :
      WidgetsContainerBase(&AppExiterForRestApps)
{
    ApplicationWithLogin*const pThisApp = qtutilsUiAppWithLogin();

    WidgetsContainerBase::AddNewWidget(a_pLoginWnd,a_login_show,nullptr,false);
    WidgetsContainerBase::AddNewWidget(a_pMainWindow,a_main_show,nullptr,true);
    WidgetsContainerBase::SetIconstForContextMenu(a_logo,a_tooltip,a_exitLogo);
    
    ConnectSignalsQuLogin();

    if(pThisApp->isVisible()){
        SetVisible();
    }
    else{
        SetHidden();
    }
	
	if(pThisApp->isLoggedIn()){
        WidgetsContainerBase::SwitchToWidget(QUTILS_WIDG_CONT_LOGIN_MAINW);
    }
    else{
        WidgetsContainerBase::SwitchToWidget(QUTILS_WIDG_CONT_LOGIN_LOGIN);
    }

}


void WidgetsContainerLogin::show()
{
    ApplicationWithLogin*const pThisApp = qtutilsUiAppWithLogin();
    if(pThisApp->isVisible()){
        WidgetsContainerBase::show();
    }
    else{
        m_flags.wr.showCalled = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
    }
}


void WidgetsContainerLogin::SetVisible()
{
    if(m_flags.rd.showCalled_true){
        WidgetsContainerBase::show();
    }
}


void WidgetsContainerLogin::SetHidden()
{
    const uint64_t cunCurFlag = m_flags.wr.isVisible;
    WidgetsContainerBase::hide();
    m_flags.wr.isVisible = cunCurFlag;
}


void WidgetsContainerLogin::ConnectSignalsQuLogin()
{
    ApplicationWithLogin*const pThisApp = qtutilsUiAppWithLogin();

    QObject::connect(pThisApp,&ApplicationWithLogin::aboutToQuit,this,&WidgetsContainerBase::hide);
    QObject::connect(pThisApp,&ApplicationWithLogin::AppGoingToExitSignal,this,&WidgetsContainerBase::hide);
    QObject::connect(pThisApp,&ApplicationWithLogin::SwitchingToHidenSignal,this,&WidgetsContainerLogin::SetHidden);
    QObject::connect(pThisApp,&ApplicationWithLogin::SwitchingToVisibleSignal,this,&WidgetsContainerLogin::SetVisible);

    QObject::connect(pThisApp,&ApplicationWithLogin::LoginSucceedSignal,pThisApp,[this](){
        WidgetsContainerBase::SwitchToWidget(QUTILS_WIDG_CONT_LOGIN_MAINW);
    });

    QObject::connect(pThisApp,&ApplicationWithLogin::LoggedOutSignal,pThisApp,[this](bool a_isAppExit){
        if(a_isAppExit){
            this->hide();
        }
        else{
            WidgetsContainerBase::SwitchToWidget(QUTILS_WIDG_CONT_LOGIN_LOGIN);
        }
    });

    QObject::connect(pThisApp,&ApplicationWithLogin::LoginFailedSignal,pThisApp,[](QString a_errorDetails){
        const QString title = "Login failed";
        const QString text = a_errorDetails;
        ::qtutils::ui::ShowMessageBox(QMessageBox::Critical,title,text);
    });
}


void WidgetsContainerLogin::MakeLoginCloseToExitApp()
{
    WidgetsContainerBase::SetWidgetHasHideFromSysMenu(QUTILS_WIDG_CONT_LOGIN_LOGIN,false);
}


void WidgetsContainerLogin::MakeLoginCloseToHideLogin()
{
    WidgetsContainerBase::SetWidgetHasHideFromSysMenu(QUTILS_WIDG_CONT_LOGIN_LOGIN,true);
}


}}  //  namespace qtutils { namespace ui{
