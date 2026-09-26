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
#include <cinternal/undisable_compiler_warnings.h>


namespace qtutils { namespace core{ namespace sql{


static inline QString QVariantToQStringForSqlInline(const QVariant& a_var){
    return (a_var.typeId()==QMetaType::QString) ? (QString("'") + a_var.toString() + "'") : a_var.toString() ;
}


static inline QString TriggerPsql01NameInline(const QString& a_functionName,const QString& a_tableName){
    const QString triggerName = "qu_trgr_ch01_fn_" +  a_functionName + "_tb_" + a_tableName;
    return triggerName;
}


static inline QString NotificationPsql01NameInline(const QString& a_functionName,const QString& a_tableName){
    const QString notifName = "qu_noty_ch01_fn_" +  a_functionName + "_tb_" + a_tableName;
    return notifName;
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


QTUTILS_EXPORT bool CreateTriggerForPsqlDbChangeAndSubscribe01(const QSqlDatabase& a_db, const QString& a_functionName,const QString& a_tableName)
{
    QSqlDriver* const driver = a_db.driver();
    if(driver){
        QSqlQuery qry(a_db);
        const QString notifName = NotificationPsql01NameInline(a_functionName,a_tableName);
        const QString triggerName = TriggerPsql01NameInline(a_functionName,a_tableName);
        const QString dropTriggerQuery = "DROP TRIGGER IF EXISTS " +triggerName+ " \n" "ON " +a_tableName+ ";";

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


QTUTILS_EXPORT void UnsuscribeAndDropTriggerForPsqlDbChange01(const QSqlDatabase& a_db, const QString& a_functionName,const QString& a_tableName)
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


}}}  //  namespace qtutils { namespace core{ namespace sql{
