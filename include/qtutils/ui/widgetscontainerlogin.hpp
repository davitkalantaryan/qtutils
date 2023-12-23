//
// repo:            qtutils
// file:            widgetscontainerlogin.hpp
// path:			include/qtutils/ui/widgetscontainerlogin.hpp
// created on:		2023 Jul 02
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#pragma once

#include <qtutils/export_symbols.h>
#include <qtutils/ui/widgetscontainerbasequ.hpp>


namespace qtutils { namespace ui{

#define QUTILS_WIDG_CONT_LOGIN_LOGIN    0
#define QUTILS_WIDG_CONT_LOGIN_MAINW    1


class QTUTILS_UI_EXPORT WidgetsContainerLogin : public WidgetsContainerBase
{

public:
    virtual ~WidgetsContainerLogin() override;
    WidgetsContainerLogin(QWidget* CPPUTILS_ARG_NN a_pLoginWnd, QWidget* CPPUTILS_ARG_NN a_pMainWindow,
                          const QString& a_logo, const QString& a_tooltip,const QString& a_exitLogo,
                          const TypeWCShowHide& a_login_show, const TypeWCShowHide& a_main_show);
    virtual void show() override;
    void MakeLoginCloseToExitApp();
    void MakeLoginCloseToHideLogin();

public slots:
    void SetVisible();
    void SetHidden();
    
protected:
    void ConnectSignalsQuLogin();
};


}}  //  namespace qtutils { namespace ui{
