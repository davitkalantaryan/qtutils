//
// repo:            qtutils
// file:            global_functions.hpp
// path:			include/qtutils/core/template_functions01.hpp
// created on:		2024 Feb 21
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once


#include <qtutils/export_symbols.h>
#include <qtutils/disable_utils_warnings.h>
#include <QList>
#include <QMap>


namespace qtutils { namespace core{


template <typename keyT, typename dataT>
static inline QMap<keyT,dataT> QListFromQPairsToMapT(const QList<QPair<keyT,dataT> >& a_list)
{
    QMap<keyT,dataT> retMap;
    for(const auto& iter: a_list){
        retMap.insert(iter.first,iter.second);
    }
    return retMap;
}


}}  // namespace qtutils { namespace core{
