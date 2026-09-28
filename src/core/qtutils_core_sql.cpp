//
// repo:            qtutils
// file:            qtutils_core_sqldbwrp.cpp
// path:			src/core/qtutils_core_sqldbwrp.hpp
// created on:		2023 Nov 21
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#include <qtutils/core/sql.hpp>
#include <cinternal/disable_compiler_warnings.h>
#include <qtutils/disable_utils_warnings.h>
#include <QSqlError>
#include <QMessageLogger>
#include <QVariantList>
#include <QVariant>
#include <QSqlDriver>
#include <QJsonDocument>
#include <QJsonObject>
#include <cinternal/undisable_compiler_warnings.h>


namespace qtutils { namespace core{ namespace sql{


#define QTUTILS_SQL_NOTY01_NAME     "qtutils_core_sql_psql_notification01"

struct SDbChangeSbscription{
    QString         subscriptionName;
    QSqlDriver*     driver;
    TypeDbChnglbk   clbk;
};


static inline QString QVariantToQStringForSqlInline(const QVariant& a_var){
    return (a_var.typeId()==QMetaType::QString) ? (QString("'") + a_var.toString() + "'") : a_var.toString() ;
}


static inline QString TriggerPsql01NameInline(const QString& a_functionName,const QString& a_tableName){
    const QString triggerName = "qu_trgr_ch01_fn_" +  a_functionName + "_tb_" + a_tableName;
    return triggerName;
}


QTUTILS_EXPORT void PrintErrorStatF(const QSqlDatabase& a_db, const QString& a_extraText, const char* a_file, int a_line, const char* a_function)
{
    const QSqlError sqlErr = a_db.lastError();
    QMessageLogger(a_file, a_line, a_function).critical()<< a_extraText << sqlErr.text();
    QMessageLogger(a_file, a_line, a_function).critical()<< "> Database error:" << sqlErr.databaseText();
    QMessageLogger(a_file, a_line, a_function).critical()<< "> Driver error:" << sqlErr.driverText();
    QMessageLogger(a_file, a_line, a_function).critical()<< "> Native error code:" << sqlErr.nativeErrorCode();
    QMessageLogger(a_file, a_line, a_function).critical()<< "> Error type" << sqlErr.type();
}


QTUTILS_EXPORT QString GetLastSqlQuery(const QSqlQuery& a_qry)
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


QTUTILS_EXPORT bool CreateTriggerFunctionForDbChangePsql01(QSqlQuery* CPPUTILS_ARG_NN a_qry_p, const QString& a_functionName)
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


QTUTILS_EXPORT void DropTriggerFunctionForDbChangePsql01(QSqlQuery* CPPUTILS_ARG_NN a_qry_p, const QString& a_functionName)
{
    const QString queryStr = "DROP FUNCTION IF EXISTS " +a_functionName+ "();";
    a_qry_p->exec(queryStr);
}


QTUTILS_EXPORT bool CreateTriggerForPsqlDbChange01(QSqlQuery* CPPUTILS_ARG_NN a_qry_p, const QString& a_functionName,const QString& a_tableName)
{
    const QString triggerName = TriggerPsql01NameInline(a_functionName,a_tableName);
    const QString dropTriggerQuery = "DROP TRIGGER IF EXISTS " +triggerName+ " \n" "ON " +a_tableName+ ";";

    if(!a_qry_p->exec(dropTriggerQuery)){
        return false;
    }

    const QString queryStr =
        "CREATE TRIGGER " +triggerName+ " \n"
         "AFTER INSERT OR UPDATE OR DELETE \n"
         "ON " +a_tableName+ " \n"
         "FOR EACH ROW \n"
         "EXECUTE FUNCTION " +a_functionName+ "('" QTUTILS_SQL_NOTY01_NAME "');";
    if(!a_qry_p->exec(queryStr)){
        return false;
    }

    return true;
}


QTUTILS_EXPORT void DropTriggerForPsqlDbChange01(QSqlQuery* CPPUTILS_ARG_NN a_qry_p, const QString& a_functionName,const QString& a_tableName)
{
    const QString triggerName = TriggerPsql01NameInline(a_functionName,a_tableName);
    const QString dropTriggerQuery = "DROP TRIGGER IF EXISTS " +triggerName+ " \n" "ON " +a_tableName+ ";";
    a_qry_p->exec(dropTriggerQuery);
}


QTUTILS_EXPORT SDbChangeSbscription* SubscribeForPsqlDbChange01(const QSqlDatabase& a_db, const TypeDbChnglbk& a_clbk)
{
    QSqlDriver* const driver = a_db.driver();
    if(driver){
        if(driver->subscribeToNotification(QTUTILS_SQL_NOTY01_NAME)){
            SDbChangeSbscription* const pRetData = new SDbChangeSbscription();
            pRetData->subscriptionName = QTUTILS_SQL_NOTY01_NAME;
            pRetData->driver = driver;
            pRetData->clbk = a_clbk;
            QObject::connect(driver,&QSqlDriver::notification,driver,[pRetData](const QString& a_name, QSqlDriver::NotificationSource a_source, const QVariant& a_payload){
                if(a_name.compare(QTUTILS_SQL_NOTY01_NAME,Qt::CaseInsensitive)==0){
                    const QJsonDocument payloadJsonDoc = QJsonDocument::fromJson(a_payload.toString().toUtf8());
                    const QJsonObject payloadJsonObj = payloadJsonDoc.object();
                    const int id = payloadJsonObj.value("id").toInt();
                    const QString tableName = payloadJsonObj.value("table").toString();
                    const QString dbOp = payloadJsonObj.value("operation").toString();
                    if(dbOp.compare("INSERT",Qt::CaseInsensitive)==0){
                        (pRetData->clbk)(tableName,DbChngOp::Insert,id);
                    }
                    else if(dbOp.compare("UPDATE",Qt::CaseInsensitive)==0){
                        (pRetData->clbk)(tableName,DbChngOp::Update,id);
                    }
                    else if(dbOp.compare("DELETE",Qt::CaseInsensitive)==0){
                        (pRetData->clbk)(tableName,DbChngOp::Delete,id);
                    }
                    else{
                        // maybe logging?
                        (void)a_source;
                        (pRetData->clbk)(tableName,DbChngOp::Unknown,id);
                    }
                }  //  if(a_name.compare(QTUTILS_SQL_NOTY01_NAME,Qt::CaseInsensitive)==0){
            });  //  QObject::connect(...)
            return pRetData;
        }  //  if(driver->subscribeToNotification(QTUTILS_SQL_NOTY01_NAME)){
    }  //  if(driver){

    return nullptr;
}


QTUTILS_EXPORT void UnsubscribeFromPsqlDbChange(SDbChangeSbscription* a_subscrpStr_p)
{
    if(a_subscrpStr_p){
        a_subscrpStr_p->driver->unsubscribeFromNotification(a_subscrpStr_p->subscriptionName);
        delete a_subscrpStr_p;
    }
}


}}}  //  namespace qtutils { namespace core{ namespace sql{
