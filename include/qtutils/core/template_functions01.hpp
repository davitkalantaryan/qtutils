//
// repo:            qtutils
// file:            global_functions.hpp
// path:			include/qtutils/core/template_functions01.hpp
// created on:		2024 Feb 21
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once


#include <qtutils/export_symbols.h>
#include <cinternal/disable_compiler_warnings.h>
#include <qtutils/disable_utils_warnings.h>
#include <QList>
#include <QMultiHash>
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
#include <QHttpHeaders>
#endif
#include <cinternal/undisable_compiler_warnings.h>


namespace qtutils { namespace core{


#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)

static inline QMultiHash<QByteArray,QByteArray> QListFromQPairsToMapT(const QHttpHeaders& a_list)
{
    return a_list.toMultiHash();
}

#else

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


}}  // namespace qtutils { namespace core{
