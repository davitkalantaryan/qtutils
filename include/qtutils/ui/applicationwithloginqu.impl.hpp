//
// repo:            qtutils
// file:            applicationwithlogin.hpp
// path:			include/ui/applicationwithlogin.hpp
// created on:		2023 Jul 01
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_APPLICATIONWITHLOGIN_IMPL_HPP
#define QTUTILS_INCLUDE_QTUTILS_UI_APPLICATIONWITHLOGIN_IMPL_HPP

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_APPLICATIONWITHLOGIN_HPP
#include <qtutils/ui/applicationwithloginqu.hpp>
#endif
#include <qtutils/disable_utils_warnings.h>
#include <QMetaObject>


namespace qtutils { namespace ui {


template <typename TypeApp>
ApplicationWithLoginBase<TypeApp>::~ApplicationWithLoginBase()
{
    m_data_p->m_flagsBS.wr.shouldRun = CPPUTILS_BISTATE_MAKE_BITS_FALSE;
    m_data_p->CleanWhileThreadsAliveByApp();
    m_data_p->StopThreadsByApp();
    m_data_p->CleanAfterThreadsByApp();
    delete m_data_p;
}


template <typename TypeApp>
template <typename... Targs>
ApplicationWithLoginBase<TypeApp>::ApplicationWithLoginBase(ApplicationWithLogin_data* CPPUTILS_ARG_NN a_data_p, Targs... a_args)
    :
      TypeApp(a_args...),
      m_data_p(a_data_p)
{
    m_data_p->m_flagsBS.wr_all = CPPUTILS_BISTATE_MAKE_ALL_BITS_FALSE;
    m_data_p->m_flagsFS.wr_all = CPPUTILS_FOURSTATE_MAKE_ALL_BITS_FALSE;
    m_data_p->m_flagsBS.wr.shouldRun = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
    
    m_data_p->InitBeforeThreadsByApp();
    m_data_p->StartThreadsByApp();
    m_data_p->InitAfterThreadsByApp();
}


template <typename TypeApp>
QString ApplicationWithLoginBase<TypeApp>::username()const
{
    return m_data_p->m_username;
}


template <typename TypeApp>
QString ApplicationWithLoginBase<TypeApp>::loginBaseUrl()const
{
    return m_data_p->m_loginBaseUrl;
}


template <typename TypeApp>
bool ApplicationWithLoginBase<TypeApp>::isVisible()const
{
    return true;
}


template <typename TypeApp>
void ApplicationWithLoginBase<TypeApp>::SetVisible()
{
}


template <typename TypeApp>
void ApplicationWithLoginBase<TypeApp>::SetInvisible()
{
}


template <typename TypeApp>
bool ApplicationWithLoginBase<TypeApp>::hasInvisiblePossibility()const
{
    return false;
}


template <typename TypeApp>
void ApplicationWithLoginBase<TypeApp>::ExitApp(const QString& a_reason)
{
    static_cast<void>(a_reason);
    ExitAppCommonPart();
    QMetaObject::invokeMethod(this,[](){
        QCoreApplication::quit();
    });
}


template <typename TypeApp>
bool ApplicationWithLoginBase<TypeApp>::isGoingToExit()const
{
    return static_cast<bool>(m_data_p->m_flagsBS.rd.shouldRun_false);
}


template <typename TypeApp>
bool ApplicationWithLoginBase<TypeApp>::isLoggedIn()const
{
    return static_cast<bool>(m_data_p->m_flagsFS.rd1.loggedIn_true);
}


template <typename TypeApp>
bool ApplicationWithLoginBase<TypeApp>::shouldLoginSpin()const
{
    return static_cast<bool>(m_data_p->m_flagsFS.rd1.loggedIn_ongoing_false_to_true);
}


template <typename TypeApp>
void ApplicationWithLoginBase<TypeApp>::showAboutInfo()const
{
}


template <typename TypeApp>
void ApplicationWithLoginBase<TypeApp>::ExitAppCommonPart()
{
    m_data_p->m_flagsBS.wr.shouldRun = CPPUTILS_BISTATE_MAKE_BITS_FALSE;
}


template <typename TypeApp>
template <typename WidgType>
void ApplicationWithLoginBase<TypeApp>::StartWidget(const QPoint& a_glbPos, WidgType** CPPUTILS_ARG_NN a_ppWidget)
{
    if(m_data_p->m_flagsBS.rd.shouldRun_true){
        QMetaObject::invokeMethod(this,[this,a_glbPos,a_ppWidget](){
            if(m_data_p->m_flagsBS.rd.shouldRun_true){
                WidgType*& pWidget = *a_ppWidget;
                if(!pWidget){
                    pWidget = new WidgType();
                    pWidget->resize(500,500);
                }
            
                pWidget->move(a_glbPos);
                pWidget->InitAndShow();
                pWidget->activateWindow();
            }  //  if(m_appbase_data_p->m_flagsBS.rd.shouldRun_true){   II
        });  //  QMetaObject::invokeMethod(this,[this,a_glbPos](){
    }  //  if(m_appbase_data_p->m_flagsBS.rd.shouldRun_true){
}


}}  //  namespace qtutils { namespace ui {


#endif  //  #ifndef QTUTILS_INCLUDE_QTUTILS_UI_APPLICATIONWITHLOGIN_IMPL_HPP
