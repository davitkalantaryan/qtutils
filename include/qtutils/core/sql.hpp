//
// file:                sqldbwrp.hpp
// path:                include/qtutils/core/sqldbwrp.hpp
// created on:          2023 Nov 21
// created by:          Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#pragma once

#include <qtutils/export_symbols.h>
#include <cinternal/disable_compiler_warnings.h>
#include <qtutils/disable_utils_warnings.h>
#include <QString>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <cinternal/undisable_compiler_warnings.h>


namespace qtutils { namespace core{ namespace sql{


#define PrintErrorStatM(_db,_extraText)  PrintErrorStatF((_db),(_extraText),__FILE__,__LINE__,__FUNCTION__)


QTUTILS_EXPORT void PrintErrorStatF(const QSqlDatabase& a_db, const QString& a_extraText, const char* a_file, int a_line, const char* a_func);
QTUTILS_EXPORT QString GetLastSqlQuery(const QSqlQuery& a_qry);



}}}  // namespace qtutils { namespace core{ namespace sql{
