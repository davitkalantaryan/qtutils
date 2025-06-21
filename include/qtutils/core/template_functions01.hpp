//
// repo:            qtutils
// file:            global_functions.hpp
// path:			include/qtutils/core/template_functions01.hpp
// created on:		2024 Feb 21
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once


#include <qtutils/export_symbols.h>
#include <qtutils/core/http_data01.hpp>
#include <cinternal/disable_compiler_warnings.h>
#include <qtutils/disable_utils_warnings.h>
#include <QNetworkRequest>
#include <QByteArray>
#include <cinternal/undisable_compiler_warnings.h>


namespace qtutils { namespace core{


#ifdef qtutils_core_QListFromQPairsToMapT_needed

#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)

static inline HeadersMap QListFromQPairsToMapT(const TypeRestHeaders& a_list)
{
    return a_list.toMultiHash();
}

#endif

#if ( QT_VERSION < QT_VERSION_CHECK(6, 7, 0) ) || defined(Generic_QListFromQPairsToMapT_needed)

template <typename keyT, typename dataT>
static inline QMultiHash<keyT,dataT> QListFromQPairsToMapT(const QList<QPair<keyT,dataT> >& a_list)
{
    QMultiHash<keyT,dataT> retMap;
    for(const auto& iter: a_list){
        retMap.insert(iter.first,iter.second);
    }
    return retMap;
}

#endif

#endif //  #ifdef qtutils_core_QListFromQPairsToMapT_needed


#ifdef qtutils_core_GetValueFromHeadersCaseInsensitive_needed

static inline QByteArray GetValueFromHeadersCaseInsensitive(const HeadersMap& a_requestHeaders, const QByteArray& a_headerName){    
    if(a_requestHeaders.contains(a_headerName)){
        return a_requestHeaders.value(a_headerName);
    }
    
    const QByteArray headerNameLC = a_headerName.toLower();
    if(a_requestHeaders.contains(headerNameLC)){
        return a_requestHeaders.value(headerNameLC);
    }
    
    const QByteArray headerNameUC = a_headerName.toUpper();
    if(a_requestHeaders.contains(headerNameUC)){
        return a_requestHeaders.value(headerNameUC);
    }
    
    const HeadersMap::const_iterator iterEnd = a_requestHeaders.constEnd();
    HeadersMap::const_iterator iter = a_requestHeaders.constBegin();
    for(;iter!=iterEnd;++iter){
        if(a_headerName.compare(iter.key(),Qt::CaseInsensitive)==0){
            return iter.value();
        }  //  if(a_headerName.compare(iter.value(),Qt::CaseInsensitive)==0){
    }  //  for(;iter!=iterEnd;++iter){
    
    return QByteArray();
}

#endif  //  #ifdef qtutils_core_GetValueFromHeadersCaseInsensitive_needed


#ifdef qtutils_core_GetValueFromNetworkRequestCaseInsensitive_needed

static inline QByteArray GetValueFromNetworkRequestCaseInsensitive(const QNetworkRequest& a_requestHeaders, const QByteArray& a_headerName){
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
    
    const QHttpHeaders aHdrs = a_requestHeaders.headers();
    const HeadersMap hdrsMap = aHdrs.toMultiHash();
    return GetValueFromHeadersCaseInsensitive(hdrsMap,a_headerName);
    
#else
    
    if(a_requestHeaders.hasRawHeader(a_headerName)){
        return a_requestHeaders.rawHeader(a_headerName);
    }
    
    const QByteArray headerNameLC = a_headerName.toLower();
    if(a_requestHeaders.hasRawHeader(headerNameLC)){
        return a_requestHeaders.rawHeader(headerNameLC);
    }
    
    const QByteArray headerNameUC = a_headerName.toUpper();
    if(a_requestHeaders.hasRawHeader(headerNameUC)){
        return a_requestHeaders.rawHeader(headerNameUC);
    }
    
    return QByteArray();
    
#endif
}

#endif  //  #ifdef focust_p01_core_JwtTokenFromNetworkRequest_needed


}}  // namespace qtutils { namespace core{
