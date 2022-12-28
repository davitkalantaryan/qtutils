//
// file:                sql.hpp
// path:                include/qtutils/core/sql.hpp
// created on:          2022 Oct 12
// created by:          Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#pragma once

#include <qtutils/export_symbols.h>

#ifdef QTUTILS_USE_NON_STANDARD_DB
#include <qtutils/core/sqldatabase.hpp>
#include <qtutils/core/sql.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QString>
#else
#include <qtutils/disable_utils_warnings.h>
#include <QSql>
#endif

#ifdef QTUTILS_USE_NON_STANDARD_DB

namespace qtutils { namespace sql{

enum ParamType{
    In = 0x00000001,
    Out = 0x00000002,
    InOut = In|Out,
    Binary = 0x00000004,
};

}}  //  namespace qtutils { namespace sql{

#else

namespace qtutils {

namespace sql=QSql;

}  //  namespace qtutils {

#endif
