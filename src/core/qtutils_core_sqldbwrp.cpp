//
// file:            qtutils_core_sqldbwrp.cpp
// path:			src/core/qtutils_core_sqldbwrp.hpp
// created on:		2023 Nov 21
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/core/sqldbwrp_p.hpp>
#include <cinternal/thread_local_sys.h>
#include <mutex>
#include <atomic>
#include <stddef.h>
#include <stdint.h>
#ifdef QTUTILS_LOGGER_IS_USED
#include <qtutils/core/logger.hpp>
#define QtUtilsCriticalMacro        QtUtilsCritical
#define QtUtilsWarningMacro         QtUtilsWarning
#define QtUtilsInfoMacro            QtUtilsInfo
#else
#include <QDebug>
#define QtUtilsCriticalMacro        qCritical
#define QtUtilsWarningMacro         qWarning
#define QtUtilsInfoMacro            qInfo
#endif
#include <qtutils/disable_utils_warnings.h>
#include <QSqlError>
#include <QMessageLogger>


namespace qtutils { namespace core{


static CinternalTlsData     s_tls_data;
class CPPUTILS_DLL_PRIVATE MutexTlsInitDeInit{
public:
    ~MutexTlsInitDeInit();
    MutexTlsInitDeInit();
}static s_mutexTlsInitDeInit;
static void PthreadKeyCreateDeleteClbk(void*){}
MutexTlsInitDeInit::~MutexTlsInitDeInit(){
    if(s_tls_data){CinternalTlsDelete(s_tls_data);}
}
MutexTlsInitDeInit::MutexTlsInitDeInit(){
    CinternalTlsAlloc(&s_tls_data,&PthreadKeyCreateDeleteClbk);
    if(s_tls_data==CINTERNAL_TLS_OUT_OF_INDEXES){
        throw "Failure on constructor. Unable to create TLS";
    }
}


static inline void PrintErrorStatRawInline(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p, const QString& a_extraText, const char* a_file, int a_line, const char* a_function)
{
    const QSqlError sqlErr = a_db_p->m_db_p->lastError();
    QMessageLogger(a_file, a_line, a_function).critical()<< a_extraText << sqlErr.text();
    QMessageLogger(a_file, a_line, a_function).critical()<< "> Database error:" << sqlErr.databaseText();
    QMessageLogger(a_file, a_line, a_function).critical()<< "> Driver error:" << sqlErr.driverText();
    QMessageLogger(a_file, a_line, a_function).critical()<< "> Native error code:" << sqlErr.nativeErrorCode();
    QMessageLogger(a_file, a_line, a_function).critical()<< "> Error type" << sqlErr.type();
}


static inline void CleanupDbInline(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p){
    if(a_db_p->m_db_p){
        if(a_db_p->m_db_p->isOpen()){
            a_db_p->m_db_p->close();
        }  //  if(a_db_p->m_db_p && a_db_p->m_db_p->isOpen()){
        delete a_db_p->m_db_p;
        a_db_p->m_db_p = nullptr;
    }  //  if(a_db_p->m_db_p && a_db_p->m_db_p->isOpen()){
    
    if(a_db_p->m_isConnectionOwnedByThis){
        SqlDatabase::removeDatabase(a_db_p->m_connectionName);
        a_db_p->m_isConnectionOwnedByThis = false;
    }
    
    a_db_p->m_connectionName = "";
    a_db_p->m_type = "";
}


static inline bool InitializeInline(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p, const QString& a_type, const QString& a_dbNameOrPath, const QString& a_hostname, const QString& a_username, const QString& a_password, int a_port, const QString* a_connectionName_p, bool a_isConnectionOwnedByThis){
    a_db_p->m_db_p = new SqlDatabase();
    a_db_p->m_type = a_type;
    a_db_p->m_isConnectionOwnedByThis = a_isConnectionOwnedByThis;
    if(a_connectionName_p){
        *a_db_p->m_db_p = SqlDatabase::addDatabase(a_type,*a_connectionName_p);
    }
    else{
        *a_db_p->m_db_p = SqlDatabase::addDatabase(a_type);
    }
    a_db_p->m_db_p->setDatabaseName(a_dbNameOrPath);
    if(a_hostname.size()>0){
        a_db_p->m_db_p->setHostName(a_hostname);
    }
    if(a_username.size()>0){
        a_db_p->m_db_p->setUserName(a_username);
    }
    if(a_password.size()>0){
        a_db_p->m_db_p->setPassword(a_password);
    }
    if(a_port>=0){
        a_db_p->m_db_p->setPort(a_port);
    }
    
    if(a_db_p->m_db_p->open()){
        a_db_p->m_connectionName = a_db_p->m_db_p->connectionName();
        return true;
    }
    
    CleanupDbInline(a_db_p);
    
    return false;
}


static inline bool CheckAndTryToReconnectDbInline(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p){
    {
        SqlQuery aQry1(*(a_db_p->m_db_p));
        if(aQry1.exec("SELECT 1;")){
            return true;
        }
    }
    
    // connection to DB lost, let's recover it
    QtUtilsWarningMacro()<<"Connection to DB lost, trying to reopen";
    const QString type = a_db_p->m_type;
    const QString connectionName = a_db_p->m_connectionName;
    const QString dbNameOrPath = a_db_p->m_db_p->databaseName();
    const QString hostname = a_db_p->m_db_p->hostName();
    const QString username = a_db_p->m_db_p->userName();
    const QString password = a_db_p->m_db_p->password();
    const int port = a_db_p->m_db_p->port();
    const bool isConnectionOwnedByThis = a_db_p->m_isConnectionOwnedByThis;
    
    CleanupDbInline(a_db_p);
    if(!InitializeInline(a_db_p,type,dbNameOrPath,hostname,username,password,port,&connectionName,isConnectionOwnedByThis)){
        QtUtilsCriticalMacro()<<"Unable reinitialize DB";
        return false;
    }
    
    
    {
        SqlQuery aQry2(a_db_p->m_db);
        if(aQry2.exec("SELECT 1;")){
            QtUtilsInfoMacro()<<"Connection to DB recovered";
            return true;
        }
    }
    
    QtUtilsCriticalMacro()<<"Unable to recover connection to db";
    return false;
}


#define LockOfTablesInlineMacro(_qry_p,_tablesNamesStr,_lockMode)   \
    (_qry_p)->exec("LOCK TABLE " + (_tablesNamesStr) + " IN " + (_lockMode) + " MODE;")


static inline QString TableNamesStrFromListInline(const QStringList& a_tablesNames){
    const qsizetype tablesCount = a_tablesNames.size();
    if(tablesCount>0){
        QString tablesNamesStr = a_tablesNames.at(0);
        if(tablesCount>1){
            for(qsizetype i(1); i<tablesCount; ++i){
                tablesNamesStr += (", " + a_tablesNames.at(i));
            }  //  for(qsizetype i(1); i<tablesCount; ++i){
        }  //  if(tablesCount>1){
        return tablesNamesStr;
    }  //  if(tablesCount>0){
    return QString();
}


static inline bool LockOfTablesInline(SqlQuery* CPPUTILS_ARG_NN a_qry_p, const QStringList& a_tablesNames, const QString& a_lockMode){
    return LockOfTablesInlineMacro(a_qry_p,TableNamesStrFromListInline(a_tablesNames),a_lockMode);
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


void SqlDbWrp::CleanupDb()
{
    ::std::lock_guard<SqlDbWrp> aGuard(*this);
    CleanupDbInline(m_db_data_p);
}


bool SqlDbWrp::Initialize(const QString& a_type, const QString& a_dbNameOrPath, const QString& a_hostname, const QString& a_username, const QString& a_password, int a_port, const QString* a_connectionName_p)
{
    ::std::lock_guard<SqlDbWrp> aGuard(*this);
    return InitializeInline(m_db_data_p,a_type,a_dbNameOrPath,a_hostname,a_username,a_password,a_port, a_connectionName_p, true);
}


bool SqlDbWrp::InitializePostgreSQL(const QString& a_dbName, const QString& a_hostname, const QString& a_username, const QString& a_password,int a_port, const QString* a_connectionName_p)
{
    ::std::lock_guard<SqlDbWrp> aGuard(*this);
    return InitializeInline(m_db_data_p,"QPSQL",a_dbName,a_hostname,a_username,a_password,a_port,a_connectionName_p, true);
}


bool SqlDbWrp::InitializeSQLite(const QString& a_dbPath, const QString* a_connectionName_p)
{
    ::std::lock_guard<SqlDbWrp> aGuard(*this);
    return InitializeInline(m_db_data_p,"QSQLITE",a_dbPath,"","","",-1,a_connectionName_p,true);
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
    PrintErrorStatRawInline(m_db_data_p,a_extraText, a_file,a_line,a_function);
}


#endif  //  #ifndef QTUTILS_DBUSE_CLASS_API


namespace db{


static inline QString GetUniqueNameForDbSavepointInline(){
    static ::std::atomic<int> s_atomicInt(0);
    const int cnUniqueNumber = s_atomicInt.fetch_add(1, std::memory_order_relaxed);
    return "UniqueNameForDbSavepoint" + QString::number(cnUniqueNumber);
}


static inline bool StartTransactionOrSaveStateInline(SqlQuery* CPPUTILS_ARG_NN a_qry_p, QString* CPPUTILS_ARG_NN a_pSavepointStr){
    if(!a_qry_p->exec("SELECT txid_current_if_assigned();")){
        return false;
    }
    
    const bool bHasTransaction = a_qry_p->first();
    if(!bHasTransaction){
        *a_pSavepointStr = QString();
        return a_qry_p->exec("BEGIN;");
    }
    
    const QVariant hasTransVar = a_qry_p->value(0);
    if(hasTransVar.isNull()){
        *a_pSavepointStr = QString();
        return a_qry_p->exec("BEGIN;");
    }
    
    *a_pSavepointStr = GetUniqueNameForDbSavepointInline();
    return a_qry_p->exec("SAVEPOINT "+ (*a_pSavepointStr) + ";");    
}


MutexPg::MutexPg(const QString& a_tablesNamesStr, const QString& a_lockMode)
    :
      m_tablesNamesStr(a_tablesNamesStr),
      m_lockMode(a_lockMode),
      m_qry_p(nullptr),
      m_bIsLockOk(false)
{
    static_cast<void>(m_reserved01);
}


MutexPg::MutexPg(const QStringList& a_tablesNames, const QString& a_lockMode)
    :
      m_tablesNamesStr(TableNamesStrFromListInline(a_tablesNames)),
      m_lockMode(a_lockMode),
      m_qry_p(nullptr),
      m_bIsLockOk(false)
{
    static_cast<void>(m_reserved01);
}


void MutexPg::lock()
{
    ptrdiff_t currentLocks = (ptrdiff_t)CinternalTlsGetSpecific(s_tls_data);
    if((++currentLocks)<2){
        const bool bIsLockOk1 = m_qry_p->exec("BEGIN;");
        if(!bIsLockOk1){return;}
    }
    
    const bool bIsLockOk2 = m_qry_p->exec("LOCK TABLE " + m_tablesNamesStr + " IN " + m_lockMode + " MODE;");
    if(!bIsLockOk2){return;}
    
    CinternalTlsSetSpecific(s_tls_data,(void*)currentLocks);
    m_bIsLockOk = true;
}


void MutexPg::unlock()
{
    ptrdiff_t currentLocks = (ptrdiff_t)CinternalTlsGetSpecific(s_tls_data);
    if(currentLocks>0){
        if((--currentLocks)<1){
            m_qry_p->exec("COMMIT;");
        }        
        
        CinternalTlsSetSpecific(s_tls_data,(void*)currentLocks);
    }  //  if(currentLocks>0){
    
    
    m_bIsLockOk = false;
}


bool MutexPg::isLockOk()const
{
    return m_bIsLockOk;
}


void MutexPg::setQuery(QSqlQuery* CPPUTILS_ARG_NN a_qry_p)const
{
    m_qry_p = a_qry_p;
}



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


QTUTILS_EXPORT bool InitializeGlb(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p, const QString& a_type, const QString& a_dbNameOrPath, const QString& a_hostname, const QString& a_username, const QString& a_password, int a_port, const QString* a_connectionName_p, bool a_isConnectionOwnedByThis)
{
    return InitializeInline(a_db_p,a_type,a_dbNameOrPath,a_hostname,a_username,a_password,a_port,a_connectionName_p, a_isConnectionOwnedByThis);
}

QTUTILS_EXPORT bool InitializePostgreSQLGlb(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p, const QString& a_dbName, const QString& a_hostname, const QString& a_username, const QString& a_password,int a_port, const QString* a_connectionName_p, bool a_isConnectionOwnedByThis)
{
    return InitializeInline(a_db_p,"QPSQL",a_dbName,a_hostname,a_username,a_password,a_port,a_connectionName_p,a_isConnectionOwnedByThis);
}


QTUTILS_EXPORT bool InitializeSQLiteGlb(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p, const QString& a_dbPath, const QString* a_connectionName_p, bool a_isConnectionOwnedByThis)
{
    return InitializeInline(a_db_p,"QSQLITE",a_dbPath,"","","",-1,a_connectionName_p,a_isConnectionOwnedByThis);
}


QTUTILS_EXPORT bool StartTransactionOrSaveStateGlb(SqlQuery* CPPUTILS_ARG_NN a_qry_p, QString* CPPUTILS_ARG_NN a_pSavepointStr)
{
    return StartTransactionOrSaveStateInline(a_qry_p, a_pSavepointStr);
}


QTUTILS_EXPORT void PrintErrorStatRawGlb(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p, const QString& a_extraText, const char* a_file, int a_line, const char* a_function)
{
    PrintErrorStatRawInline(a_db_p,a_extraText, a_file,a_line,a_function);
}


QTUTILS_EXPORT bool CheckAndTryToReconnectDbGlb(SqlDbWrpBase_p* CPPUTILS_ARG_NN a_db_p)
{
    return CheckAndTryToReconnectDbInline(a_db_p);
}


QTUTILS_EXPORT bool LockOfTablesGlb2(SqlQuery* CPPUTILS_ARG_NN a_qry_p, const QStringList& a_tablesNames, const QString& a_lockMode)
{
    return LockOfTablesInline(a_qry_p,a_tablesNames,a_lockMode);
}


QTUTILS_EXPORT bool LockOfTablesGlbRaw(SqlQuery* CPPUTILS_ARG_NN a_qry_p, const QString& a_tablesNamesStr, const QString& a_lockMode)
{
    return LockOfTablesInlineMacro(a_qry_p,a_tablesNamesStr,a_lockMode);
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
