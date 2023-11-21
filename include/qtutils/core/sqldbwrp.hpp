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


typedef QSqlDatabase    SqlDatabase;
typedef QSqlQuery       SqlQuery;


class QTUTILS_EXPORT SqlDbWrp_p;


class QTUTILS_EXPORT SqlDbWrp
{
public:
    virtual ~SqlDbWrp();
    SqlDbWrp();
    
    void CleanupDb();
    bool Initialize(const QString& a_type, const QString& a_dbNameOrPath, const QString& a_hostname, const QString& a_username, const QString& a_password);
    bool InitializePostgreSQL(const QString& a_dbName, const QString& a_hostname, const QString& a_username, const QString& a_password);
    bool InitializeSQLite(const QString& a_dbPath);
    
    const SqlDatabase& getQtSqlDb()const;
    SqlDatabase& getQtSqlDb();
    void Commit();
    
    void lock();
    void unlock();
    
protected:
    SqlDbWrp_p* const m_db_data_p;
};


}}  //  namespace qtutils { namespace core{
