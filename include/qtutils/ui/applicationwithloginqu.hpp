//
// repo:            qtutils
// file:            applicationwithlogin.hpp
// path:			include/ui/applicationwithlogin.hpp
// created on:		2023 Jul 01
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#include <qtutils/export_symbols.h>
#include <qtutils/disable_utils_warnings.h>
#include <QApplication>
#include <QPoint>
#include <QString>


namespace qtutils { namespace ui {


class QTUTILS_EXPORT ApplicationWithLogin : public QApplication
{
	Q_OBJECT    
	
public:
    virtual ~ApplicationWithLogin() override;
    ApplicationWithLogin(int& a_argc, char** a_argv);

    virtual QString username()const = 0;
    virtual void ExitApp() = 0;
    virtual void Login(const QString& a_username, const QString& a_password) =0;
    virtual void Logout() = 0;
    virtual bool isLoggedIn()const = 0;
    virtual void ShowSettingsWnd(const QPoint& a_glbPos) = 0;
    virtual void ShowLogsWnd(const QPoint& a_glbPos) = 0;
		
private:
signals:
    void AppGoingToExitSignal();
    void LoginSucceedSignal();
    void LoggedOutSignal(bool a_isAppExit);
    void LoginFailedSignal(QString errorString);
    void StartLoginSpinnngSignal();
    void StopLoginSpinnngSignal();
};


#define qtutilsUiAppWithLogin()   static_cast< ::qtutils::ui::ApplicationWithLogin*>(qApp)


}}  //  namespace qtutils { namespace ui {
