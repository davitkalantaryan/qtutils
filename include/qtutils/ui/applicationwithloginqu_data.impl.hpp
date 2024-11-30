//
// repo:            qtutils
// file:            applicationwithlogin.hpp
// path:			include/ui/applicationwithlogin.hpp
// created on:		2023 Jul 01
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_APPLICATIONWITHLOGIN_DATA_IMPL_HPP
#define QTUTILS_INCLUDE_QTUTILS_UI_APPLICATIONWITHLOGIN_DATA_IMPL_HPP

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_APPLICATIONWITHLOGIN_DATA_HPP
#include <qtutils/ui/applicationwithloginqu_data.hpp>
#endif
#include <qtutils/disable_utils_warnings.h>
#include <QMetaObject>
#include <QCoreApplication>


namespace qtutils { namespace ui {


template <typename ObjType>
void ApplicationWithLogin_data::CreateObject(ObjType** CPPUTILS_ARG_NN a_ppObject)
{
    if(m_flagsBS.rd.shouldRun_true){
        QMetaObject::invokeMethod(qApp,[this,a_ppObject](){
            if(m_flagsBS.rd.shouldRun_true){
                ObjType*& pObject = *a_ppObject;
                if(!pObject){
                    pObject = new ObjType();
                }
            }  //  if(m_appbase_data_p->m_flagsBS.rd.shouldRun_true){   II
        });  //  QMetaObject::invokeMethod(this,[this,a_glbPos](){
    }  //  if(m_appbase_data_p->m_flagsBS.rd.shouldRun_true){
}


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


#endif  //  #ifndef QTUTILS_INCLUDE_QTUTILS_UI_APPLICATIONWITHLOGIN_DATA_IMPL_HPP
