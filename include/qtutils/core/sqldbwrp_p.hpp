//
// file:                sqldbwrp_p.hpp
// path:                include/qtutils/core/sqldbwrp_p.hpp
// created on:          2023 Nov 21
// created by:          Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#pragma once

#include <qtutils/export_symbols.h>
#include <qtutils/core/sqldbwrp.hpp>
#define cinternal_lw_recursive_mutex_create_needed
#include <cinternal/lw_mutex_recursive.h>
#undef cinternal_lw_recursive_mutex_create_needed


namespace qtutils { namespace core{



struct SqlDbWrpBase_p{   
    SqlDatabase*                    m_db_p;
    QString                         m_type;
    QString                         m_connectionName;
    bool                            m_isConnectionOwnedByThis;
};

#ifndef QTUTILS_DBUSE_CLASS_API

class QTUTILS_EXPORT SqlDbWrp_p : public SqlDbWrpBase_p
{   
public:
    cinternal_lw_recursive_mutex_t  m_mutex;
};


#endif  //  #ifndef QTUTILS_DBUSE_CLASS_API


}}  //  namespace qtutils { namespace core{
