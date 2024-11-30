//
// repo:            qtutils
// file:            applicationwithloginqu_data.hpp
// path:			include/ui/applicationwithloginqu_data.hpp
// created on:		2023 Jul 01
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_APPLICATIONWITHLOGIN_DATA_HPP
#define QTUTILS_INCLUDE_QTUTILS_UI_APPLICATIONWITHLOGIN_DATA_HPP

#include <qtutils/export_symbols.h>
#include <cinternal/bistateflags.h>
#include <cinternal/fourstateflags.h>
#include <qtutils/disable_utils_warnings.h>
#include <QString>


namespace qtutils { namespace ui {


#define QTUTILS_USERNAME_SETT_KEY        "core/auth/username"
#define QTUTILS_LOGINURL_SETT_KEY        "core/url/login"


CPPUTILS_BISTATE_FLAGS_UN_NM(
        BistateFlags,
        shouldRun,doNotCollectEarlyLogs, cleanCalled,
        isSocketConnected, isServiceSocketConnected, isSocket2Connected, isSocket3Connected,
        isClockedIn, isKeybMonitorRunning, isMouseMonitorRunning, keyStoringEnabled,
        shouldLoginWithSystemEvent, appMonitorEnabled, keybMonitorEnabled,mouseMonitorEnabled,
        clockInEnabled,shouldSpinClockin,shouldSpinBreak,screenshotEnabled,
        signalInited);
CPPUTILS_FOURSTATE_FLAGS_UN_NM(
        FourstateFlags,
        loggedIn, clockedIn, inBreak);


class QTUTILS_EXPORT ApplicationWithLogin_data
{
public:
    virtual ~ApplicationWithLogin_data() = default;
    ApplicationWithLogin_data() = default;
    ApplicationWithLogin_data(const ApplicationWithLogin_data&) = delete;
    ApplicationWithLogin_data(ApplicationWithLogin_data&&) = delete;
    ApplicationWithLogin_data& operator=(const ApplicationWithLogin_data&) = delete;
    ApplicationWithLogin_data& operator=(ApplicationWithLogin_data&&) = delete;
    
    virtual void InitBeforeThreadsByApp();
    virtual void InitAfterThreadsByApp();
    virtual void CleanWhileThreadsAliveByApp();
    virtual void CleanAfterThreadsByApp();
    virtual void StartThreadsByApp();
    virtual void StopThreadsByApp();
    
    template <typename ObjType>
    void CreateObject(ObjType** CPPUTILS_ARG_NN a_ppWidget);
    
public:
    QString             m_username;
    QString             m_loginBaseUrl;
    BistateFlags        m_flagsBS;
    FourstateFlags      m_flagsFS;
};


template <typename WidgType>
static inline void ShutdownWindowInline(WidgType** CPPUTILS_ARG_NN a_ppWidget);


}}  //  namespace qtutils { namespace ui {


#ifndef QTUTILS_INCLUDE_QTUTILS_UI_APPLICATIONWITHLOGIN_DATA_IMPL_HPP
#include <qtutils/ui/applicationwithloginqu_data.impl.hpp>
#endif


#endif  //  #ifndef QTUTILS_INCLUDE_QTUTILS_UI_APPLICATIONWITHLOGIN_DATA_HPP
