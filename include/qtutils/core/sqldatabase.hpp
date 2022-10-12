//
// file:                sqldatabase.hpp
// path:                include/qtutils/core/sqldatabase.hpp
// created on:          2022 Oct 12
// created by:          Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#pragma once

#include <qtutils/export_symbols.h>

#ifdef QTUTILS_USE_NON_STANDARD_DB
#include <qtutils/core/sqlerror.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QString>
#include <QLatin1String>
#else
#include <qtutils/disable_utils_warnings.h>
#include <QSqlDatabase>
#endif


namespace qtutils {

#ifdef QTUTILS_USE_NON_STANDARD_DB

class CPPUTILS_DLL_PRIVATE SqlDatabase_p;

class QTUTILS_EXPORT SqlDatabase
{
public:
    SqlDatabase();
    SqlDatabase(const SqlDatabase& cM);
    SqlDatabase(SqlDatabase&& cM);
    ~SqlDatabase();
    SqlDatabase operator=(const SqlDatabase& cM);
    SqlDatabase operator=(SqlDatabase&& cM);

    bool        isOpen() const;
    bool        open();
    void        close();
    void        setDatabaseName(const QString& name);
    SqlError    lastError() const;
    bool        commit();

    static SqlDatabase	addDatabase(const QString &type, const QString& connectionName =  QLatin1String(defaultConnection));
    static void	removeDatabase(const QString& connectionName);

public:
    static const char* defaultConnection;

private:
    SqlDatabase_p*     m_db_data_p;
};

#else

typedef QSqlDatabase SqlDatabase;

#endif

}  //  namespace qtutils {
