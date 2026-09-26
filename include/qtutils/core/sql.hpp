//
// file:                sqldbwrp.hpp
// path:                include/qtutils/core/sqldbwrp.hpp
// created on:          2023 Nov 21
// created by:          Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#pragma once

#include <qtutils/export_symbols.h>
#include <cinternal/disable_compiler_warnings.h>
#include <functional>
#include <qtutils/disable_utils_warnings.h>
#include <QString>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <cinternal/undisable_compiler_warnings.h>


namespace qtutils { namespace core{ namespace sql{


#define qtutilsCoreSqlPrintErrorStatM(_db,_extraText)  \
    qtutils::core::sql::PrintErrorStatF((_db),(_extraText),__FILE__,__LINE__,__FUNCTION__)
#define qtutilsCoreSqlPrintErrorStatSmplM(_db)  qtutilsCoreSqlPrintErrorStatM((_db),"")


typedef ::std::function<void(const QString& a_name, QSqlDriver::NotificationSource a_source, const QVariant& a_payload)>    TypeDbChnglbk;
struct SDbChangeSbscription;


QTUTILS_EXPORT void PrintErrorStatF(const QSqlDatabase& a_db, const QString& a_extraText, const char* a_file, int a_line, const char* a_func);
QTUTILS_EXPORT QString GetLastSqlQuery(const QSqlQuery& a_qry);
QTUTILS_EXPORT bool CreateTriggerFunctionForDbChangePsql01(QSqlQuery* CPPUTILS_ARG_NN a_qry_p, const QString& a_functionName);
QTUTILS_EXPORT void DropTriggerFunctionForDbChangePsql01(QSqlQuery* CPPUTILS_ARG_NN a_qry_p, const QString& a_functionName);
QTUTILS_EXPORT bool CreateTriggerForPsqlDbChange01(QSqlQuery* CPPUTILS_ARG_NN a_qry_p, const QString& a_functionName,const QString& a_tableName);
QTUTILS_EXPORT void DropTriggerForPsqlDbChange01(QSqlQuery* CPPUTILS_ARG_NN a_qry_p, const QString& a_functionName,const QString& a_tableName);
QTUTILS_EXPORT SDbChangeSbscription* SubscribeForPsqlDbChange01(const QSqlDatabase& a_db, const TypeDbChnglbk& a_clbk);
QTUTILS_EXPORT void UnsubscribeFromPsqlDbChange(SDbChangeSbscription* a_subscrpStr_p);



}}}  // namespace qtutils { namespace core{ namespace sql{
