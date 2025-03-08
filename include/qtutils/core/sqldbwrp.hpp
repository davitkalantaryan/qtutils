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
#include <QStringList>


namespace qtutils { namespace core{


#define QUSqlPrintErrorStat(_extraText)  PrintErrorStatRaw((_extraText),__FILE__,__LINE__,__FUNCTION__)
#define QUSqlPrintErrorStatGlb(_db_p,_extraText)  PrintErrorStatRawGlb((_db_p),(_extraText),__FILE__,__LINE__,__FUNCTION__)


typedef QSqlDatabase    SqlDatabase;
typedef QSqlQuery       SqlQuery;
class QTUTILS_EXPORT SqlDbWrp_p;
struct SqlDbWrpBase_p;


namespace db{

QTUTILS_EXPORT QString GetLastSqlQuery(const SqlQuery& a_qry);
QTUTILS_EXPORT bool StartTransactionOrSaveStateGlb(SqlQuery* CPPUTILS_ARG_NN a_qry_p, QString* CPPUTILS_ARG_NN a_pSavepointStr);
QTUTILS_EXPORT bool CheckAndTryToReconnectDbGlb(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p);
QTUTILS_EXPORT bool LockOfTablesGlb2(SqlQuery* CPPUTILS_ARG_NN a_qry_p, const QStringList& a_tablesNames, const QString& a_lockMode = QString("SHARE ROW EXCLUSIVE"));
QTUTILS_EXPORT bool LockOfTablesGlbRaw(SqlQuery* CPPUTILS_ARG_NN a_qry_p, const QString& a_tablesNames, const QString& a_lockMode = QString("SHARE ROW EXCLUSIVE"));
QTUTILS_EXPORT void PrintErrorStatRawGlb(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p, const QString& a_extraText, const char* a_file, int a_line, const char* a_function);
QTUTILS_EXPORT void CleanupDbGlb(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p);
QTUTILS_EXPORT bool InitializeGlb(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p, const QString& a_type, const QString& a_dbNameOrPath, const QString& a_hostname, const QString& a_username, const QString& a_password, int a_port, const QString* a_connectionName_p=nullptr, bool a_isConnectionOwnedByThis=true);
QTUTILS_EXPORT bool InitializePostgreSQLGlb(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p, const QString& a_dbName, const QString& a_hostname, const QString& a_username, const QString& a_password,int a_port, const QString* a_connectionName_p=nullptr,bool a_isConnectionOwnedByThis=true);
QTUTILS_EXPORT bool InitializeSQLiteGlb(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p, const QString& a_dbPath, const QString* a_connectionName_p=nullptr,bool a_isConnectionOwnedByThis=true);
QTUTILS_EXPORT bool CloneAndOpenDbGlb(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p, const SqlDbWrpBase_p& a_dbInp, const QString& a_connectionName, bool a_isConnectionOwnedByThis=true);
QTUTILS_EXPORT bool CloneAndOpenDbGlb(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p, const QString& a_oldConnectionName, const QString& a_newConnectionName, bool a_isConnectionOwnedByThis=true);


class QTUTILS_EXPORT MutexPg
{
public:
    MutexPg(const QString& a_tablesNamesStr, const QString& a_lockMode = QString("SHARE ROW EXCLUSIVE"));
    MutexPg(const QStringList& a_tablesNames, const QString& a_lockMode = QString("SHARE ROW EXCLUSIVE"));
    MutexPg(const MutexPg&)=delete;
    MutexPg& operator=(const MutexPg&)=delete;
    
    void lock();
    void unlock();
    bool isLockOk()const;
    void setQuery(QSqlQuery* CPPUTILS_ARG_NN a_qry_p)const;
    
private:
    const QString       m_tablesNamesStr;
    const QString       m_lockMode;
    mutable QSqlQuery*  m_qry_p;
    bool                m_bIsLockOk;
    bool                m_reserved01[7];
};


}  //  namespace db{


#ifndef QTUTILS_DBUSE_CLASS_API

class QTUTILS_EXPORT SqlDbWrp
{
public:
    virtual ~SqlDbWrp();
    SqlDbWrp();
    
    void CleanupDb();
    bool Initialize(const QString& a_type, const QString& a_dbNameOrPath, const QString& a_hostname, const QString& a_username, const QString& a_password, int a_port, const QString* a_connectionName_p=nullptr);
    bool InitializePostgreSQL(const QString& a_dbName, const QString& a_hostname, const QString& a_username, const QString& a_password,int a_port, const QString* a_connectionName_p=nullptr);
    bool InitializeSQLite(const QString& a_dbPath, const QString* a_connectionName_p=nullptr);
    bool CloneAndOpenDb(const SqlDbWrpBase_p& a_dbInp, const QString& a_connectionName);
    bool CloneAndOpenDb(const QString& a_oldConnectionName, const QString& a_newConnectionName);
    bool copyThisToBase(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p,const QString& a_connectionName)const;
    bool copyThisToOther(SqlDbWrp* CPPUTILS_ARG_NN a_other,const QString& a_connectionName)const;
    
    const SqlDatabase& getQtSqlDb()const;
    SqlDatabase& getQtSqlDb();
    void Commit();

    void PrintErrorStatRaw(const QString& a_extraText, const char* a_file, int a_line, const char* a_function);
    
    void lock();
    void unlock();

    SqlDbWrpBase_p* getBasePointer()const;
    
protected:
    SqlDbWrp_p* const m_db_data_p;
};


#endif  //  #ifndef QTUTILS_DBUSE_CLASS_API


}}  //  namespace qtutils { namespace core{
