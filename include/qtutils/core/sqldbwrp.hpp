//
// file:                sqldbwrp.hpp
// path:                include/qtutils/core/sqldbwrp.hpp
// created on:          2023 Nov 21
// created by:          Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#pragma once

#include <qtutils/export_symbols.h>
#include <qtutils/disable_utils_warnings.h>
#include <QString>
#include <QSqlQuery>
#include <QSqlDatabase>


namespace qtutils { namespace core{


#define QUSqlPrintErrorStat(_extraText)  PrintErrorStatRaw((_extraText),__FILE__,__LINE__,__FUNCTION__)
#define QUSqlPrintErrorStatGlb(_db_p,_extraText)  PrintErrorStatRawGlb((_db_p),(_extraText),__FILE__,__LINE__,__FUNCTION__)


typedef QSqlDatabase    SqlDatabase;
typedef QSqlQuery       SqlQuery;
class QTUTILS_EXPORT SqlDbWrp_p;
class QTUTILS_EXPORT SqlDbWrpBase_p;


namespace db{

QTUTILS_EXPORT QString GetLastSqlQuery(const SqlQuery& a_qry);
QTUTILS_EXPORT bool StartTransactionGlb(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p, SqlQuery* CPPUTILS_ARG_NN a_qry_p);
QTUTILS_EXPORT void PrintErrorStatRawGlb(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p, const QString& a_extraText, const char* a_file, int a_line, const char* a_function);
QTUTILS_EXPORT void CleanupDbGlb(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p);
QTUTILS_EXPORT bool InitializeGlb(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p, const QString& a_type, const QString& a_dbNameOrPath, const QString& a_hostname, const QString& a_username, const QString& a_password, int a_port, const QString* a_connectionName_p=nullptr);
QTUTILS_EXPORT bool InitializePostgreSQLGlb(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p, const QString& a_dbName, const QString& a_hostname, const QString& a_username, const QString& a_password,int a_port, const QString* a_connectionName_p=nullptr);
QTUTILS_EXPORT bool InitializeSQLiteGlb(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p, const QString& a_dbPath, const QString* a_connectionName_p=nullptr);
QTUTILS_EXPORT bool CloneAndOpenDbGlb(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p, const SqlDbWrpBase_p& a_dbInp, const QString& a_connectionName);
QTUTILS_EXPORT bool CloneAndOpenDbGlb(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p, const QString& a_oldConnectionName, const QString& a_newConnectionName);

}  //  namespace db{


#ifndef QTUTILS_DBUSE_CLASS_API

class QTUTILS_EXPORT SqlDbWrp
{
public:
    virtual ~SqlDbWrp();
    SqlDbWrp();
    
    bool StartTransaction(SqlQuery* CPPUTILS_ARG_NN a_qry_p);
    void CleanupDb();
    bool Initialize(const QString& a_type, const QString& a_dbNameOrPath, const QString& a_hostname, const QString& a_username, const QString& a_password, int a_port, const QString* a_connectionName_p=nullptr);
    bool InitializePostgreSQL(const QString& a_dbName, const QString& a_hostname, const QString& a_username, const QString& a_password,int a_port, const QString* a_connectionName_p=nullptr);
    bool InitializeSQLite(const QString& a_dbPath, const QString* a_connectionName_p=nullptr);
    bool CloneAndOpenDb(const SqlDbWrpBase_p& a_dbInp, const QString& a_connectionName);
    bool CloneAndOpenDb(const QString& a_oldConnectionName, const QString& a_newConnectionName);
    
    const SqlDatabase& getQtSqlDb()const;
    SqlDatabase& getQtSqlDb();
    void Commit();

    void PrintErrorStatRaw(const QString& a_extraText, const char* a_file, int a_line, const char* a_function);
    
    void lock();
    void unlock();
    
protected:
    SqlDbWrp_p* const m_db_data_p;
};


#endif  //  #ifndef QTUTILS_DBUSE_CLASS_API


}}  //  namespace qtutils { namespace core{
