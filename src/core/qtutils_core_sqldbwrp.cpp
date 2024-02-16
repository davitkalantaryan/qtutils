//
// file:            qtutils_core_sqldbwrp.cpp
// path:			src/core/qtutils_core_sqldbwrp.hpp
// created on:		2023 Nov 21
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/core/sqldbwrp_p.hpp>
#include <mutex>
#include <qtutils/disable_utils_warnings.h>
#include <QSqlError>
#include <QMessageLogger>


namespace qtutils { namespace core{


SqlDbWrp::~SqlDbWrp()
{
    SqlDbWrp::CleanupDb();
    cinternal_lw_recursive_mutex_destroy(&(m_db_data_p->m_mutex));
    delete m_db_data_p;
}


SqlDbWrp::SqlDbWrp()
    :
      m_db_data_p(new SqlDbWrp_p())
{
    cinternal_lw_recursive_mutex_create(&(m_db_data_p->m_mutex));
}


void SqlDbWrp::CleanupDb()
{
    ::std::lock_guard<SqlDbWrp> aGuard(*this);
    if(m_db_data_p->m_db.isOpen()){
        m_db_data_p->m_db.close();
        SqlDatabase::removeDatabase(m_db_data_p->m_type);
        m_db_data_p->m_type = "";
    }
}


bool SqlDbWrp::Initialize(const QString& a_type, const QString& a_dbNameOrPath, const QString& a_hostname, const QString& a_username, const QString& a_password, int a_port)
{
    ::std::lock_guard<SqlDbWrp> aGuard(*this);
    m_db_data_p->m_type = a_type;
    m_db_data_p->m_db = SqlDatabase::addDatabase(a_type);
    m_db_data_p->m_db.setDatabaseName(a_dbNameOrPath);
    if(a_hostname.size()>0){
        m_db_data_p->m_db.setHostName(a_hostname);
    }
    if(a_username.size()>0){
        m_db_data_p->m_db.setUserName(a_username);
    }
    if(a_password.size()>0){
        m_db_data_p->m_db.setPassword(a_password);
    }
    if(a_port>=0){
        m_db_data_p->m_db.setPort(a_port);
    }
    
    if(m_db_data_p->m_db.open()){
        return true;
    }
    
    SqlDatabase::removeDatabase(a_type);
    m_db_data_p->m_type = "";
    return false;
}


bool SqlDbWrp::InitializePostgreSQL(const QString& a_dbName, const QString& a_hostname, const QString& a_username, const QString& a_password,int a_port)
{
    return SqlDbWrp::Initialize("QPSQL",a_dbName,a_hostname,a_username,a_password,a_port);
}


bool SqlDbWrp::InitializeSQLite(const QString& a_dbPath)
{
    return SqlDbWrp::Initialize("QSQLITE",a_dbPath,"","","",-1);
}


const SqlDatabase& SqlDbWrp::getQtSqlDb()const
{
    return m_db_data_p->m_db;
}


SqlDatabase& SqlDbWrp::getQtSqlDb()
{
    return m_db_data_p->m_db;
}


void SqlDbWrp::lock()
{
    cinternal_lw_recursive_mutex_lock(&(m_db_data_p->m_mutex));
}


void SqlDbWrp::unlock()
{
    cinternal_lw_recursive_mutex_unlock(&(m_db_data_p->m_mutex));
}


void SqlDbWrp::Commit()
{
    if(m_db_data_p->m_type=="QSQLITE"){
        m_db_data_p->m_db.commit();
    }
}


void SqlDbWrp::PrintErrorStatRaw(const QString& a_extraText, const char* a_file, int a_line, const char* a_function)
{
    const QSqlError sqlErr = m_db_data_p->m_db.lastError();
    QMessageLogger(a_file, a_line, a_function).critical()<< a_extraText << sqlErr.text();
    QMessageLogger(a_file, a_line, a_function).critical()<< "> Database error:" << sqlErr.databaseText();
    QMessageLogger(a_file, a_line, a_function).critical()<< "> Driver error:" << sqlErr.driverText();
    QMessageLogger(a_file, a_line, a_function).critical()<< "> Native error code:" << sqlErr.nativeErrorCode();
    QMessageLogger(a_file, a_line, a_function).critical()<< "> Error type" << sqlErr.type();
}



}}  // namespace qtutils { namespace core{
