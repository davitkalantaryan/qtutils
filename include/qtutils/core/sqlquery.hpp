//
// file:                sqlquery.hpp
// path:                include/qtutils/core/sqlquery.hpp
// created on:          2022 Oct 12
// created by:          Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#pragma once

#include <qtutils/export_symbols.h>

#ifdef QTUTILS_USE_NON_STANDARD_DB
#include <qtutils/core/sql.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QString>
#include <QVariant>
#else
#include <qtutils/disable_utils_warnings.h>
#include <QSqlQuery>
#endif


namespace qtutils {

#ifdef QTUTILS_USE_NON_STANDARD_DB

class SqlDatabase;
class CPPUTILS_DLL_PRIVATE SqlQuery_p;

class QTUTILS_EXPORT SqlQuery
{
public:
    ~SqlQuery();
    SqlQuery(const SqlDatabase& db);

    bool        exec(const QString& query);
    bool        exec();
    bool        prepare(const QString& query);
    void        addBindValue(const QVariant &val, sql::ParamType paramType = sql::In);
    bool        first();
    bool        next();
    QVariant    value(int index) const;
    int         numRowsAffected() const;

private:
    SqlQuery_p* m_sql_querry_data_p;
};

#else

typedef QSqlQuery SqlQuery;

#endif

}  //  namespace qtutils {
