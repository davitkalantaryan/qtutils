//
// repo:            qtutils
// file:            applicationwithloginqu_data.hpp
// path:			include/ui/applicationwithloginqu_data.hpp
// created on:		2023 Jul 01
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

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
        shouldRun,doNotCollectEarlyLogs);
CPPUTILS_FOURSTATE_FLAGS_UN_NM(
        FourstateFlags,
        loggedIn, clockedIn);


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
    
public:
    QString             m_username;
    QString             m_loginBaseUrl;
    BistateFlags        m_flagsBS;
    FourstateFlags      m_flagsFS;
};


template <typename WidgType>
static inline void ShutdownWindowInline(WidgType** CPPUTILS_ARG_NN a_ppWidget){
    WidgType*& pWidget = *a_ppWidget;
    if(pWidget){
        WidgType* const pWidgetTmp = pWidget;
        pWidget = nullptr;
        pWidgetTmp->setHidden(true);
        pWidgetTmp->deleteLater();
    }
}


}}  //  namespace qtutils { namespace ui {
