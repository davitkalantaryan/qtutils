//
// file:            qtutils_core_sqldbwrp.cpp
// path:			src/core/qtutils_core_sqldbwrp.hpp
// created on:		2023 Nov 21
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/core/sqldbwrp_p.hpp>
#include <mutex>


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


bool SqlDbWrp::Initialize(const QString& a_type, const QString& a_dbNameOrPath, const QString& a_hostname, const QString& a_username, const QString& a_password)
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
    
    if(m_db_data_p->m_db.open()){
        return true;
    }
    
    SqlDatabase::removeDatabase(a_type);
    m_db_data_p->m_type = "";
    return false;
}


bool SqlDbWrp::InitializePostgreSQL(const QString& a_dbName, const QString& a_hostname, const QString& a_username, const QString& a_password)
{
    return SqlDbWrp::Initialize("QPSQL",a_dbName,a_hostname,a_username,a_password);
}


bool SqlDbWrp::InitializeSQLite(const QString& a_dbPath)
{
    return SqlDbWrp::Initialize("QSQLITE",a_dbPath,"","","");
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


/*/////////////////////////////////////////////////////////////////////////////////////////*/

void SqlDbWrp_p::Commit()
{
    if(m_type=="QSQLITE"){
        m_db.commit();
    }
}


}}  // namespace qtutils { namespace core{
