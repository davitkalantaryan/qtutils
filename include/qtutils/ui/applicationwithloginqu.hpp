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
#include <qtutils/ui/applicationwithloginqu_data.hpp>
#include <qtutils/core/networkaccessmanagerqu.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QString>
#include <QSettings>
#if defined(QT_WIDGETS_LIB)
#include <QApplication>
#include <QPoint>
#include <QSize>
#elif defined(QT_GUI_LIB)
#include <QGuiApplication>
#elif defined(QT_CORE_LIB)
#include <QCoreApplication>
#else
#error At least qt gui should be used
#endif


namespace qtutils { namespace ui {


template <typename TypeApp>
class ApplicationWithLoginBase : public TypeApp
{	
public:
    virtual ~ApplicationWithLoginBase() override;
    template <typename... Targs>
    ApplicationWithLoginBase(ApplicationWithLogin_data* CPPUTILS_ARG_NN a_data_p, Targs... a_args);

    virtual QString username()const;
    virtual QString loginBaseUrl()const;
    virtual void ExitApp(const QString& a_reason);
    virtual void Login(const QString& a_username, const QString& a_password) =0;
    virtual void Logout() = 0;
    virtual bool isLoggedIn()const ;
    virtual bool shouldLoginSpin()const;
    virtual bool isGoingToExit()const;
    virtual bool isVisible()const;
    virtual void SetVisible();
    virtual void SetInvisible();
    virtual bool hasInvisiblePossibility()const;
    virtual void showAboutInfo()const;
    void ExitAppCommonPart();
    template <typename ObjType>
    void CreateObject(ObjType** CPPUTILS_ARG_NN a_ppWidget);
    template <typename WidgType>
    void CreateAndShowAnyWidget(const QPoint& a_glbPos, const QSize& a_size, WidgType** CPPUTILS_ARG_NN a_ppWidget);
    template <typename WidgType>
    void CreateAndShowSpecWidget(const QPoint& a_glbPos, const QSize& a_size, WidgType** CPPUTILS_ARG_NN a_ppWidget);
    
private:
    void CleanData();
    
protected:
    ApplicationWithLogin_data* const        m_data_p;
};


#define QTUTILS_DEFINE_LOGIN_APP_FNCS(_classname,_baseClass)                        \
public:                                                                             \
    virtual ~_classname() override = default;                                       \
    using ApplicationWithLoginBase<_baseClass>::ApplicationWithLoginBase;           \
    virtual void SetUsername(const QString& a_username) {                \
        if(a_username!=m_data_p->m_username){   \
            QSettings aSettings;    \
            m_data_p->m_username = a_username;  \
            aSettings.setValue(QTUTILS_USERNAME_SETT_KEY,a_username);    \
            emit UsernameChangedSignal(a_username); \
        }   \
    }   \
    virtual void SetLoginBaseUrl(const QString& a_newUrl) {                \
        const QString newUrl = ::qtutils::network::CorectUrl(a_newUrl);             \
        if(newUrl!=m_data_p->m_loginBaseUrl){   \
            QSettings aSettings;    \
            m_data_p->m_loginBaseUrl = newUrl;  \
            aSettings.setValue(QTUTILS_LOGINURL_SETT_KEY,newUrl);    \
            emit LoginBaseUrlChangedSignal(newUrl); \
        }   \
    }   \
protected:

#define QTUTILS_DEFINE_LOGIN_APP_SIGS(_classname,_baseClass)                        \
    void AppGoingToExitSignal();                                                    \
    void UsernameChangedSignal(QString a_username);                                 \
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
#define qtutilsUiAppWithLoginCore()   static_cast< ::qtutils::ui::ApplicationWithLoginCore*>(qApp)

    
#ifdef QT_GUI_LIB
class QTUTILS_EXPORT ApplicationWithLoginGui : public ApplicationWithLoginBase<QGuiApplication>
{
    Q_OBJECT
    QTUTILS_DEFINE_LOGIN_APP_FNCS(ApplicationWithLoginGui, QGuiApplication)
signals:
    QTUTILS_DEFINE_LOGIN_APP_SIGS(ApplicationWithLoginGui, QGuiApplication)
};
#define qtutilsUiAppWithLoginGui()   static_cast< ::qtutils::ui::ApplicationWithLoginGui*>(qApp)
#endif
    
    
#ifdef QT_WIDGETS_LIB  
class QTUTILS_EXPORT ApplicationWithLoginWidget : public ApplicationWithLoginBase<QApplication>
{
    Q_OBJECT
public:
    virtual void ShowSettingsWnd(const QPoint& a_glbPos, const QSize& a_size) = 0;
    virtual void ShowLogsWnd(const QPoint& a_glbPos, const QSize& a_size) = 0;
    QTUTILS_DEFINE_LOGIN_APP_FNCS(ApplicationWithLoginWidget, QApplication)
signals:
    QTUTILS_DEFINE_LOGIN_APP_SIGS(ApplicationWithLoginWidget, QApplication)
};
#define qtutilsUiAppWithLoginWidget()   static_cast< ::qtutils::ui::ApplicationWithLoginWidget*>(qApp)
#endif


}}  //  namespace qtutils { namespace ui {


#ifndef QTUTILS_INCLUDE_QTUTILS_UI_APPLICATIONWITHLOGIN_IMPL_HPP
#include <qtutils/ui/applicationwithloginqu.impl.hpp>
#endif


#endif  //  #ifndef QTUTILS_INCLUDE_QTUTILS_UI_APPLICATIONWITHLOGIN_HPP
