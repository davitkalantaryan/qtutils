//
// repo:            qtutils
// file:            main_qtutils_test_dblisten_using_core_test.cpp
// path:			src/tests/main_qtutils_test_dblisten_using_core_test.hpp
// created on:		2026 Sep 26
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//


#include <qtutils/core/sql.hpp>
#include <qtutils/core/logger.hpp>
#include <cinternal/disable_compiler_warnings.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <qtutils/disable_utils_warnings.h>
#include <QCoreApplication>
#include <QThread>
#include <cinternal/undisable_compiler_warnings.h>


#define QTUTILS_SQL_NOTY_TEST_FUNCTION01_ON_PSQL_CHANGE     "notify_tbl_chng01_test02"
#define QTUTILS_DB_LISTENER_TEST_CONN1_NAME                 "db_listener_test02_connection"


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


using namespace ::qtutils::core::sql;


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

    if(!CreateTriggerForPsqlDbChange01(qry_p,QTUTILS_SQL_NOTY_TEST_FUNCTION01_ON_PSQL_CHANGE,"bu_settings")){
        DropTriggerFunctionForDbChangePsql01(qry_p,QTUTILS_SQL_NOTY_TEST_FUNCTION01_ON_PSQL_CHANGE);
        delete qry_p;
        db_p->close();
        delete db_p;
        QSqlDatabase::removeDatabase(QTUTILS_DB_LISTENER_TEST_CONN1_NAME);
        // exit app
        SignalHandler(0);
        return;
    }

    if(!CreateTriggerForPsqlDbChange01(qry_p,QTUTILS_SQL_NOTY_TEST_FUNCTION01_ON_PSQL_CHANGE,"access_code")){
        DropTriggerForPsqlDbChange01(qry_p,QTUTILS_SQL_NOTY_TEST_FUNCTION01_ON_PSQL_CHANGE,"bu_settings");
        DropTriggerFunctionForDbChangePsql01(qry_p,QTUTILS_SQL_NOTY_TEST_FUNCTION01_ON_PSQL_CHANGE);
        delete qry_p;
        db_p->close();
        delete db_p;
        QSqlDatabase::removeDatabase(QTUTILS_DB_LISTENER_TEST_CONN1_NAME);
        // exit app
        SignalHandler(0);
        return;
    }

    SDbChangeSbscription* const pSubscrbRes = SubscribeForPsqlDbChange01(*db_p,[](const QString& a_name, QSqlDriver::NotificationSource a_source, const QVariant& a_payload){
        QtUtilsDebug().noquote().nospace()<<"a_name:"<<a_name<<",a_source:"<<a_source<<",a_payload:"<<a_payload;
    });

    if(!pSubscrbRes){
        DropTriggerForPsqlDbChange01(qry_p,QTUTILS_SQL_NOTY_TEST_FUNCTION01_ON_PSQL_CHANGE,"access_code");
        DropTriggerForPsqlDbChange01(qry_p,QTUTILS_SQL_NOTY_TEST_FUNCTION01_ON_PSQL_CHANGE,"bu_settings");
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

    QtUtilsInfoV()<<"Listener started. Press Ctrl+C to stop";

    QThread::run();

    UnsubscribeFromPsqlDbChange(pSubscrbRes);
    QSqlQuery* const qryFnl_p = new QSqlQuery(*db_p);
    DropTriggerForPsqlDbChange01(qryFnl_p,QTUTILS_SQL_NOTY_TEST_FUNCTION01_ON_PSQL_CHANGE,"access_code");
    DropTriggerForPsqlDbChange01(qryFnl_p,QTUTILS_SQL_NOTY_TEST_FUNCTION01_ON_PSQL_CHANGE,"bu_settings");
    DropTriggerFunctionForDbChangePsql01(qryFnl_p,QTUTILS_SQL_NOTY_TEST_FUNCTION01_ON_PSQL_CHANGE);
    delete qryFnl_p;
    db_p->close();
    delete db_p;
    QSqlDatabase::removeDatabase(QTUTILS_DB_LISTENER_TEST_CONN1_NAME);
}
