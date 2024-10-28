//
// file:            qtutils_core_sqldbwrp.cpp
// path:			src/core/qtutils_core_sqldbwrp.hpp
// created on:		2023 Nov 21
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/core/sqldbwrp_p.hpp>
#include <mutex>
#ifdef QTUTILS_LOGGER_IS_USED
#include <qtutils/core/logger.hpp>
#define QtUtilsCriticalMacro        QtUtilsCritical
#define QtUtilsWarningMacro         QtUtilsWarning
#else
#include <QDebug>
#define QtUtilsCriticalMacro        qCritical
#define QtUtilsWarningMacro         qWarning
#endif
#include <qtutils/disable_utils_warnings.h>
#include <QSqlError>
#include <QMessageLogger>


namespace qtutils { namespace core{


static inline void CleanupDbInline(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p){
    if(a_db_p->m_db.isOpen()){
        a_db_p->m_db.close();
        SqlDatabase::removeDatabase(a_db_p->m_connectionName);
        a_db_p->m_connectionName = "";
        a_db_p->m_type = "";
    }
}


static inline bool InitializeInline(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p, const QString& a_type, const QString& a_dbNameOrPath, const QString& a_hostname, const QString& a_username, const QString& a_password, int a_port, const QString* a_connectionName_p){
    a_db_p->m_type = a_type;
    if(a_connectionName_p){
        a_db_p->m_db = SqlDatabase::addDatabase(a_type,*a_connectionName_p);
    }
    else{
        a_db_p->m_db = SqlDatabase::addDatabase(a_type);
    }
    a_db_p->m_db.setDatabaseName(a_dbNameOrPath);
    if(a_hostname.size()>0){
        a_db_p->m_db.setHostName(a_hostname);
    }
    if(a_username.size()>0){
        a_db_p->m_db.setUserName(a_username);
    }
    if(a_password.size()>0){
        a_db_p->m_db.setPassword(a_password);
    }
    if(a_port>=0){
        a_db_p->m_db.setPort(a_port);
    }
    
    if(a_db_p->m_db.open()){
        a_db_p->m_connectionName = a_db_p->m_db.connectionName();
        return true;
    }
    
    const QString connectionName = a_connectionName_p ? (*a_connectionName_p) : a_db_p->m_db.connectionName();
    SqlDatabase::removeDatabase(connectionName);
    a_db_p->m_type = "";
    return false;
}


static inline bool StartTransactionInline(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p, SqlQuery* CPPUTILS_ARG_NN a_qry_p){
    if(a_qry_p->exec("BEGIN;")){
        return true;
    }
    
    // connection to DB lost, let's recover it
    QtUtilsWarningMacro()<<"Connection to DB lost, trying to reopen";
    const QString type = a_db_p->m_type;
    const QString connectionName = a_db_p->m_connectionName;
    const QString dbNameOrPath = a_db_p->m_db.databaseName();
    const QString hostname = a_db_p->m_db.hostName();
    const QString username = a_db_p->m_db.userName();
    const QString password = a_db_p->m_db.password();
    const int port = a_db_p->m_db.port();
    
    CleanupDbInline(a_db_p);
    if(!InitializeInline(a_db_p,type,dbNameOrPath,hostname,username,password,port,&connectionName)){
        QtUtilsCriticalMacro()<<"Unable reinitialize DB";
        return false;
    }
    
    if(a_qry_p->exec("BEGIN;")){ // we will make query atomic
        return true;
    }
    
    QtUtilsCriticalMacro()<<"Unable start transaction";
    return false;
}


static inline bool LockOfTablesInline(SqlQuery* CPPUTILS_ARG_NN a_qry_p, const QStringList& a_tablesNames, const QString& a_lockMode){
    const qsizetype tablesCount = a_tablesNames.size();
    if(tablesCount>0){
        QString execStr = "LOCK TABLE " + a_tablesNames.at(0);
        if(tablesCount>1){
            for(qsizetype i(1); i<tablesCount; ++i){
                execStr += (", " + a_tablesNames.at(i));
            }  //  for(qsizetype i(1); i<tablesCount; ++i){
        }  //  if(tablesCount>1){
        execStr += " IN " + a_lockMode + " MODE;";
        return a_qry_p->exec(execStr);
    }  //  if(tablesCount>0){
    return false;
}


static inline bool CloneAndOpenDbInline(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p, const SqlDbWrpBase_p& a_dbInp, const QString& a_connectionName){
    a_db_p->m_db = QSqlDatabase::cloneDatabase(a_dbInp.m_db,a_connectionName);
    if(!a_db_p->m_db.isOpen()){
        a_db_p->m_db.open();
    }
    if(a_db_p->m_db.isOpen()){
        a_db_p->m_type = a_db_p->m_db.driverName();
        a_db_p->m_connectionName = a_db_p->m_db.connectionName();
        return true;
    }
    return false;
}


static inline bool CloneAndOpenDbInline(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p, const QString& a_oldConnectionName, const QString& a_newConnectionName){
    a_db_p->m_db = QSqlDatabase::cloneDatabase(a_oldConnectionName,a_newConnectionName);
    if(!a_db_p->m_db.isOpen()){
        a_db_p->m_db.open();
    }
    if(a_db_p->m_db.isOpen()){
        a_db_p->m_type = a_db_p->m_db.driverName();
        a_db_p->m_connectionName = a_db_p->m_db.connectionName();
        return true;
    }
    return false;
}


static inline QString QVariantToQStringForSqlInline(const QVariant& a_var){
    return (a_var.typeId()==QMetaType::QString) ? (QString("'") + a_var.toString() + "'") : a_var.toString() ;
}


#ifndef QTUTILS_DBUSE_CLASS_API

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


bool SqlDbWrp::StartTransaction(SqlQuery* CPPUTILS_ARG_NN a_qry_p)
{
    return StartTransactionInline(m_db_data_p,a_qry_p);
}


void SqlDbWrp::CleanupDb()
{
    ::std::lock_guard<SqlDbWrp> aGuard(*this);
    CleanupDbInline(m_db_data_p);
}


bool SqlDbWrp::Initialize(const QString& a_type, const QString& a_dbNameOrPath, const QString& a_hostname, const QString& a_username, const QString& a_password, int a_port, const QString* a_connectionName_p)
{
    ::std::lock_guard<SqlDbWrp> aGuard(*this);
    return InitializeInline(m_db_data_p,a_type,a_dbNameOrPath,a_hostname,a_username,a_password,a_port, a_connectionName_p);
}


bool SqlDbWrp::InitializePostgreSQL(const QString& a_dbName, const QString& a_hostname, const QString& a_username, const QString& a_password,int a_port, const QString* a_connectionName_p)
{
    ::std::lock_guard<SqlDbWrp> aGuard(*this);
    return InitializeInline(m_db_data_p,"QPSQL",a_dbName,a_hostname,a_username,a_password,a_port,a_connectionName_p);
}


bool SqlDbWrp::InitializeSQLite(const QString& a_dbPath, const QString* a_connectionName_p)
{
    ::std::lock_guard<SqlDbWrp> aGuard(*this);
    return InitializeInline(m_db_data_p,"QSQLITE",a_dbPath,"","","",-1,a_connectionName_p);
}


bool SqlDbWrp::CloneAndOpenDb(const SqlDbWrpBase_p& a_dbInp, const QString& a_connectionName)
{
    return CloneAndOpenDbInline(m_db_data_p,a_dbInp,a_connectionName);
}


bool SqlDbWrp::CloneAndOpenDb(const QString& a_oldConnectionName, const QString& a_newConnectionName)
{
    return CloneAndOpenDbInline(m_db_data_p,a_oldConnectionName,a_newConnectionName);
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


#endif  //  #ifndef QTUTILS_DBUSE_CLASS_API


namespace db{

QTUTILS_EXPORT QString GetLastSqlQuery(const SqlQuery& a_qry)
{
    // see: https://doc.qt.io/qt-6/qsqlquery.html#prepare
    QString debugQueryString = a_qry.lastQuery();

    // first let's find the type of binding (Oracle style or ODBC)
    qsizetype i;
    qsizetype index = debugQueryString.indexOf('?',0);
    if(index<0){
        // probably we have Oracle style :name
        bool scanNotStopped;
        index = 0;
        while(1){
            index = debugQueryString.indexOf(':',index);
            if(index<0){
                break;
            }
            const QString rmnStr = debugQueryString.mid(index);
            const qsizetype rmnStrLen = rmnStr.length();
            scanNotStopped = true;
            for(i=0;i<rmnStrLen;){
                const QVariant value = a_qry.boundValue(rmnStr.left(++i));
                if(value.isValid()){
                    const QString replaceString = QVariantToQStringForSqlInline(value);
                    debugQueryString.replace(index, i, replaceString);
                    scanNotStopped = false;
                    break;
                }
            }
            if(scanNotStopped){
                return debugQueryString;
            }
            ++index;

        }  //  while(1){
    }  //  if(index<0){
    else{
        // we have ODBC ?
        const QVariantList bvals = a_qry.boundValues();
        const qsizetype valsCount = bvals.size();
        if(valsCount>0){
            debugQueryString.replace(index++, 1, QVariantToQStringForSqlInline(bvals[0]));
            for(i=1; i<valsCount;++i){
                index = debugQueryString.indexOf('?',index);
                if(index<0){
                    break;
                }
                debugQueryString.replace(index++, 1, QVariantToQStringForSqlInline(bvals[i]));
            }  //  for(i=1; i<valsCount;++i){
        }
    }

    return debugQueryString;
}


QTUTILS_EXPORT bool InitializeGlb(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p, const QString& a_type, const QString& a_dbNameOrPath, const QString& a_hostname, const QString& a_username, const QString& a_password, int a_port, const QString* a_connectionName_p)
{
    return InitializeInline(a_db_p,a_type,a_dbNameOrPath,a_hostname,a_username,a_password,a_port,a_connectionName_p);
}

QTUTILS_EXPORT bool InitializePostgreSQLGlb(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p, const QString& a_dbName, const QString& a_hostname, const QString& a_username, const QString& a_password,int a_port, const QString* a_connectionName_p)
{
    return InitializeInline(a_db_p,"QPSQL",a_dbName,a_hostname,a_username,a_password,a_port,a_connectionName_p);
}


QTUTILS_EXPORT bool InitializeSQLiteGlb(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p, const QString& a_dbPath, const QString* a_connectionName_p)
{
    return InitializeInline(a_db_p,"QSQLITE",a_dbPath,"","","",-1,a_connectionName_p);
}


QTUTILS_EXPORT bool StartTransactionGlb(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p, SqlQuery* CPPUTILS_ARG_NN a_qry_p)
{
    return StartTransactionInline(a_db_p,a_qry_p);
}


QTUTILS_EXPORT bool LockOfTablesGlb(SqlQuery* CPPUTILS_ARG_NN a_qry_p, const QStringList& a_tablesNames, const QString& a_lockMode)
{
    return LockOfTablesInline(a_qry_p,a_tablesNames,a_lockMode);
}


QTUTILS_EXPORT void CleanupDbGlb(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p)
{
    CleanupDbInline(a_db_p);
}


QTUTILS_EXPORT bool CloneAndOpenDbGlb(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p, const SqlDbWrpBase_p& a_dbInp, const QString& a_connectionName)
{
    return CloneAndOpenDbInline(a_db_p,a_dbInp,a_connectionName);
}


QTUTILS_EXPORT bool CloneAndOpenDbGlb(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p,const QString& a_oldConnectionName, const QString& a_newConnectionName)
{
    return CloneAndOpenDbInline(a_db_p,a_oldConnectionName,a_newConnectionName);
}


}  //  namespace db{


}}  // namespace qtutils { namespace core{
