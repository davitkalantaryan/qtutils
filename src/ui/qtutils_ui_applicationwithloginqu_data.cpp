//
// file:			qtutils_ui_logwnd.cpp
// path:			src/ui/qtutils_ui_logwnd.cpp
// created on:		2022 Oct 13
// created by:		Davit Kalantaryan (davit.kalantaryan1980@gmail.com)
//


#include <qtutils/ui/applicationwithloginqu_data.hpp>
#include <qtutils/core/networkaccessmanagerqu.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QSettings>


namespace qtutils{ namespace ui{


void ApplicationWithLogin_data::InitBeforeThreadsByApp()
{
    const QSettings aSettings;
    
    m_username = ::qtutils::network::CorectUrl(aSettings.value(QTUTILS_USERNAME_SETT_KEY,"").toString());
    m_loginBaseUrl = ::qtutils::network::CorectUrl(aSettings.value(QTUTILS_LOGINURL_SETT_KEY,"https://login.smartdav.de:1443").toString());
}


void ApplicationWithLogin_data::InitAfterThreadsByApp()
{
    //
}


void ApplicationWithLogin_data::CleanWhileThreadsAliveByApp()
{
    //
}


void ApplicationWithLogin_data::CleanAfterThreadsByApp()
{
    //
}


void ApplicationWithLogin_data::StartThreadsByApp()
{
    //
}


void ApplicationWithLogin_data::StopThreadsByApp()
{
    //
}



}}  //  namespace qtutils{ namespace ui{
