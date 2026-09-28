//
// repo:            qtutils
// file:            main_qtutils_test_dblisten_test.cpp
// path:			src/tests/main_qtutils_test_dblisten_test.hpp
// created on:		2026 Sep 26
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//


#include <qtutils/core/logger.hpp>
#include <cinternal/disable_compiler_warnings.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <qtutils/disable_utils_warnings.h>
#include <QCoreApplication>
#include <QThread>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlDriver>
#include <cinternal/undisable_compiler_warnings.h>


#define QTUTILS_SQL_NOTY_TEST_FUNCTION01_ON_PSQL_CHANGE  "notify_tbl_chng01_test"
#define QTUTILS_DB_LISTENER_TEST_CONN1_NAME     "db_listener_test_connection"


class CPPUTILS_DLL_PRIVATE DbListenThread final : public QThread
{
    void run() override;
};


static void InitSignals(void) noexcept;


int main(int a_argc, char* a_argv[])
{
    fprintf(stdout,"Press any key, then enter to continue ");
    fflush(stdout);
    (void)getchar();

    QCoreApplication aApp(a_argc, a_argv);
    InitSignals();

    DbListenThread aThr;
    aThr.start();
    aApp.exec();
    aThr.quit();
    aThr.wait();
    fprintf(stdout,"\n");
    fflush(stdout);

    return 0;
}


static void SignalHandler(int) noexcept
{
    QCoreApplication* const pCoreApp = qApp;
    if(pCoreApp){
        QMetaObject::invokeMethod(pCoreApp,[](){
            QCoreApplication::quit();
        });
    }  //  if(pCoreApp){
    else{
        _exit(1);
    }
}


static void InitSignals(void) noexcept
{
#ifdef _WIN32
    (void)signal(SIGINT,&SignalHandler);
    (void)signal(SIGTERM,&SignalHandler);
#else
    struct sigaction new_action;
    new_action.sa_handler = &SignalHandler;
    sigemptyset (&new_action.sa_mask);
    new_action.sa_flags = 0;
    sigaction (SIGINT, &new_action, nullptr);
    sigaction (SIGTERM, &new_action, nullptr);
    sigaction (SIGUSR1, &new_action, nullptr);
#endif
}


static bool CreateTriggerFunctionForDbChangePsql01(QSqlQuery* CPPUTILS_ARG_NN a_qry_p, const QString& a_functionName);
static void DropTriggerFunctionForDbChangePsql01(QSqlQuery* CPPUTILS_ARG_NN a_qry_p, const QString& a_functionName);
static bool CreateTriggerForPsqlDbChangeAndSubscribe01(const QSqlDatabase& a_db, const QString& a_functionName,const QString& a_tableName);
static void UnsuscribeAndDropTriggerForPsqlDbChange01(const QSqlDatabase& a_db, const QString& a_functionName,const QString& a_tableName);


void DbListenThread::run()
{
    QSqlDatabase* const db_p = new QSqlDatabase(QSqlDatabase::addDatabase("QPSQL",QTUTILS_DB_LISTENER_TEST_CONN1_NAME));
    db_p->setDatabaseName("focustagent");
    db_p->setHostName("localhost");
    db_p->setUserName("focustagent");
    db_p->setPassword("pass2");
    db_p->setPort(5432);
    if(!db_p->open()){
        delete db_p;
        QSqlDatabase::removeDatabase(QTUTILS_DB_LISTENER_TEST_CONN1_NAME);
        // exit app
        SignalHandler(0);
        return;
    }

    QSqlQuery* const qry_p = new QSqlQuery(*db_p);

    if(!CreateTriggerFunctionForDbChangePsql01(qry_p,QTUTILS_SQL_NOTY_TEST_FUNCTION01_ON_PSQL_CHANGE)){
        delete qry_p;
        db_p->close();
        delete db_p;
        QSqlDatabase::removeDatabase(QTUTILS_DB_LISTENER_TEST_CONN1_NAME);
        // exit app
        SignalHandler(0);
        return;
    }

    if(!CreateTriggerForPsqlDbChangeAndSubscribe01(*db_p,QTUTILS_SQL_NOTY_TEST_FUNCTION01_ON_PSQL_CHANGE,"bu_settings")){
        DropTriggerFunctionForDbChangePsql01(qry_p,QTUTILS_SQL_NOTY_TEST_FUNCTION01_ON_PSQL_CHANGE);
        delete qry_p;
        db_p->close();
        delete db_p;
        QSqlDatabase::removeDatabase(QTUTILS_DB_LISTENER_TEST_CONN1_NAME);
        // exit app
        SignalHandler(0);
        return;
    }

    if(!CreateTriggerForPsqlDbChangeAndSubscribe01(*db_p,QTUTILS_SQL_NOTY_TEST_FUNCTION01_ON_PSQL_CHANGE,"access_code")){
        UnsuscribeAndDropTriggerForPsqlDbChange01(*db_p,QTUTILS_SQL_NOTY_TEST_FUNCTION01_ON_PSQL_CHANGE,"bu_settings");
        DropTriggerFunctionForDbChangePsql01(qry_p,QTUTILS_SQL_NOTY_TEST_FUNCTION01_ON_PSQL_CHANGE);
        delete qry_p;
        db_p->close();
        delete db_p;
        QSqlDatabase::removeDatabase(QTUTILS_DB_LISTENER_TEST_CONN1_NAME);
        // exit app
        SignalHandler(0);
        return;
    }

    QSqlDriver* const driver = db_p->driver();
    if(!driver){
        UnsuscribeAndDropTriggerForPsqlDbChange01(*db_p,QTUTILS_SQL_NOTY_TEST_FUNCTION01_ON_PSQL_CHANGE,"access_code");
        UnsuscribeAndDropTriggerForPsqlDbChange01(*db_p,QTUTILS_SQL_NOTY_TEST_FUNCTION01_ON_PSQL_CHANGE,"bu_settings");
        DropTriggerFunctionForDbChangePsql01(qry_p,QTUTILS_SQL_NOTY_TEST_FUNCTION01_ON_PSQL_CHANGE);
        delete qry_p;
        db_p->close();
        delete db_p;
        QSqlDatabase::removeDatabase(QTUTILS_DB_LISTENER_TEST_CONN1_NAME);
        // exit app
        SignalHandler(0);
        return;
    }

    delete qry_p;

    QObject dmObj;
    QObject::connect(driver,&QSqlDriver::notification,&dmObj,[](const QString& a_name, QSqlDriver::NotificationSource a_source, const QVariant& a_payload){
        QtUtilsDebug().noquote().nospace()<<"a_name:"<<a_name<<",a_source:"<<a_source<<",a_payload:"<<a_payload;
    });

    QtUtilsInfoV()<<"Listener started. Press Ctrl+C to stop";

    QThread::run();

    QSqlQuery* const qryFnl_p = new QSqlQuery(*db_p);
    UnsuscribeAndDropTriggerForPsqlDbChange01(*db_p,QTUTILS_SQL_NOTY_TEST_FUNCTION01_ON_PSQL_CHANGE,"access_code");
    UnsuscribeAndDropTriggerForPsqlDbChange01(*db_p,QTUTILS_SQL_NOTY_TEST_FUNCTION01_ON_PSQL_CHANGE,"bu_settings");
    DropTriggerFunctionForDbChangePsql01(qryFnl_p,QTUTILS_SQL_NOTY_TEST_FUNCTION01_ON_PSQL_CHANGE);
    delete qryFnl_p;
    db_p->close();
    delete db_p;
    QSqlDatabase::removeDatabase(QTUTILS_DB_LISTENER_TEST_CONN1_NAME);
}


static bool CreateTriggerFunctionForDbChangePsql01(QSqlQuery* CPPUTILS_ARG_NN a_qry_p, const QString& a_functionName)
{
    const QString queryStr =
        "CREATE OR REPLACE FUNCTION " +a_functionName+ "() \n"
        "RETURNS trigger \n"
        "LANGUAGE plpgsql \n"
        "AS $$ \n"
        "DECLARE \n"
        "    row_id integer; \n"
        "BEGIN \n"
        "    IF TG_OP = 'DELETE' THEN \n"
        "        row_id := OLD.id; \n"
        "ELSE \n"
        "    row_id := NEW.id; \n"
        "END IF; \n"
        "PERFORM pg_notify( \n"
        "    TG_ARGV[0], \n"
        "    json_build_object( \n"
        "        'operation', TG_OP, \n"
        "        'table', TG_TABLE_NAME, \n"
        "        'id', row_id \n"
        "        )::text \n"
        "    ); \n"
        "RETURN COALESCE(NEW, OLD); \n"
        "END; \n"
        "$$;";
    return a_qry_p->exec(queryStr);
}


static void DropTriggerFunctionForDbChangePsql01(QSqlQuery* CPPUTILS_ARG_NN a_qry_p, const QString& a_functionName)
{
    const QString queryStr = "DROP FUNCTION IF EXISTS " +a_functionName+ "();";
    a_qry_p->exec(queryStr);
}


static inline QString TriggerPsql01NameInline(const QString& a_functionName,const QString& a_tableName){
    const QString triggerName = "qu_trgr_ch01_fn_" +  a_functionName + "_tb_" + a_tableName;
    return triggerName;
}


static inline QString NotificationPsql01NameInline(const QString& a_functionName,const QString& a_tableName){
    const QString notifName = "qu_noty_ch01_fn_" +  a_functionName + "_tb_" + a_tableName;
    return notifName;
}


static bool CreateTriggerForPsqlDbChangeAndSubscribe01(const QSqlDatabase& a_db, const QString& a_functionName,const QString& a_tableName)
{
    QSqlDriver* const driver = a_db.driver();
    if(driver){
        QSqlQuery qry(a_db);
        const QString notifName = NotificationPsql01NameInline(a_functionName,a_tableName);
        const QString triggerName = TriggerPsql01NameInline(a_functionName,a_tableName);

        const QString dropTriggerQuery =
            "DROP TRIGGER IF EXISTS " +triggerName+ " \n"
            "ON " +a_tableName+ ";";
        if(!qry.exec(dropTriggerQuery)){
            return false;
        }

        const QString queryStr =
            "CREATE TRIGGER " +triggerName+ " \n"
            "AFTER INSERT OR UPDATE OR DELETE \n"
            "ON " +a_tableName+ " \n"
            "FOR EACH ROW \n"
            "EXECUTE FUNCTION " +a_functionName+ "('" +notifName+ "');";
        if(!qry.exec(queryStr)){
            return false;
        }

        if(!(driver->subscribeToNotification(notifName))){
            qry.exec(dropTriggerQuery);
            return false;
        }
        return true;
    }  //  if(driver){
    return false;
}


static void UnsuscribeAndDropTriggerForPsqlDbChange01(const QSqlDatabase& a_db, const QString& a_functionName,const QString& a_tableName)
{
    QSqlDriver* const driver = a_db.driver();
    if(driver){
        QSqlQuery qry(a_db);
        const QString notifName = NotificationPsql01NameInline(a_functionName,a_tableName);
        const QString triggerName = TriggerPsql01NameInline(a_functionName,a_tableName);
        const QString dropTriggerQuery = "DROP TRIGGER IF EXISTS " +triggerName+ " \n" "ON " +a_tableName+ ";";
        driver->unsubscribeFromNotification(notifName);
        qry.exec(dropTriggerQuery);
    }  //  if(driver){
}
