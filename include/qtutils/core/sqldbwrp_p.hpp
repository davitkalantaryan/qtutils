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


class QTUTILS_EXPORT SqlDbWrp_p
{   
public:
    cinternal_lw_recursive_mutex_t  m_mutex;
    SqlDatabase                     m_db;
    QString                         m_type;
};


}}  //  namespace qtutils { namespace core{
