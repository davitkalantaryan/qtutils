//
// repo:            qtutils
// file:            applicationwithlogin.hpp
// path:			include/ui/applicationwithlogin.hpp
// created on:		2023 Jul 01
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_APPLICATIONWITHLOGIN_HPP
#define QTUTILS_INCLUDE_QTUTILS_UI_APPLICATIONWITHLOGIN_HPP

#include <qtutils/export_symbols.h>
#include <qtutils/disable_utils_warnings.h>
#include <QApplication>
#include <QPoint>
#include <QString>


namespace qtutils { namespace ui {


template <typename TypeApp>
class ApplicationWithLoginBase : public TypeApp
{	
public:
    virtual ~ApplicationWithLoginBase() override = default;
    using TypeApp::TypeApp;

    virtual QString username()const = 0;
    virtual void ExitApp(const QString& a_reason) = 0;
    virtual void Login(const QString& a_username, const QString& a_password) =0;
    virtual void Logout() = 0;
    virtual bool isLoggedIn()const = 0;
    virtual bool shouldLoginSpin()const = 0;
    virtual void ShowSettingsWnd(const QPoint& a_glbPos) = 0;
    virtual void ShowLogsWnd(const QPoint& a_glbPos) = 0;
    virtual QString loginBaseUrl()const = 0;
    virtual void SetLoginBaseUrl(const QString& a_newUrl) = 0;
    virtual bool isGoingToExit()const = 0;
    virtual bool isVisible()const;
    virtual void SetVisible();
    virtual void SetInvisible();
    virtual bool hasInvisiblePossibility()const;
};


#define QTUTILS_DEFINE_LOGIN_APP_FNCS(_classname,_baseClass)                        \
public:                                                                             \
    virtual ~_classname() override = default;                                       \
    using ApplicationWithLoginBase<_baseClass>::ApplicationWithLoginBase;           \
protected:

#define QTUTILS_DEFINE_LOGIN_APP_SIGS(_classname,_baseClass)                        \
    void AppGoingToExitSignal();                                                    \
    void LoginBaseUrlChangedSignal(QString a_newUrl);                               \
    void LoginSucceedSignal();                                                      \
    void LoggedOutSignal();                                                         \
    void LoginFailedSignal(QString errorString);                                    \
    void StartLoginSpinningSignal();                                                \
    void StopLoginSpinningSignal();                                                 \
    void SwitchingToVisibleSignal();                                                \
    void SwitchingToHidenSignal();


class QTUTILS_EXPORT ApplicationWithLoginCore : public ApplicationWithLoginBase<QCoreApplication>
{
    Q_OBJECT
    QTUTILS_DEFINE_LOGIN_APP_FNCS(ApplicationWithLoginCore, QCoreApplication)
signals:
    QTUTILS_DEFINE_LOGIN_APP_SIGS(ApplicationWithLoginCore, QCoreApplication)
};

    
class QTUTILS_EXPORT ApplicationWithLoginGui : public ApplicationWithLoginBase<QGuiApplication>
{
    Q_OBJECT
    QTUTILS_DEFINE_LOGIN_APP_FNCS(ApplicationWithLoginGui, QGuiApplication)
signals:
    QTUTILS_DEFINE_LOGIN_APP_SIGS(ApplicationWithLoginGui, QGuiApplication)
};
    
    
class QTUTILS_EXPORT ApplicationWithLoginWidget : public ApplicationWithLoginBase<QApplication>
{
    Q_OBJECT
    QTUTILS_DEFINE_LOGIN_APP_FNCS(ApplicationWithLoginWidget, QApplication)
signals:
    QTUTILS_DEFINE_LOGIN_APP_SIGS(ApplicationWithLoginWidget, QApplication)
};


#define qtutilsUiAppWithLoginCore()   static_cast< ::qtutils::ui::ApplicationWithLoginCore*>(qApp)
#define qtutilsUiAppWithLoginGui()   static_cast< ::qtutils::ui::ApplicationWithLoginGui*>(qApp)
#define qtutilsUiAppWithLoginWidget()   static_cast< ::qtutils::ui::ApplicationWithLoginWidget*>(qApp)


}}  //  namespace qtutils { namespace ui {


#ifndef QTUTILS_INCLUDE_QTUTILS_UI_APPLICATIONWITHLOGIN_IMPL_HPP
#include <qtutils/ui/applicationwithloginqu.impl.hpp>
#endif


#endif  //  #ifndef QTUTILS_INCLUDE_QTUTILS_UI_APPLICATIONWITHLOGIN_HPP
