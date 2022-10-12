//
// file:            qtutils_core_sqlquery.cpp
// path:			src/core/qtutils_core_sqlquery.cpp
// created on:		2022 Oct 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/core/sqlquery.hpp>

#ifdef QTUTILS_USE_NON_STANDARD_DB

#include <qtutils/core/sqldatabase.hpp>


namespace qtutils {

class CPPUTILS_DLL_PRIVATE SqlQuery_p
{
};



SqlQuery::~SqlQuery()
{
    delete m_sql_querry_data_p;
}



SqlQuery::SqlQuery(const SqlDatabase& a_db)
    :
      m_sql_querry_data_p(new SqlQuery_p())
{
    static_cast<void>(a_db);
}


bool SqlQuery::exec(const QString& a_query)
{
    static_cast<void>(a_query);
    return true;
}


bool SqlQuery::exec()
{
    return true;
}


bool SqlQuery::prepare(const QString& a_query)
{
    static_cast<void>(a_query);
    return true;
}


void SqlQuery::addBindValue(const QVariant& a_val, sql::ParamType a_paramType)
{
    static_cast<void>(a_val);
    static_cast<void>(a_paramType);
}


bool SqlQuery::first()
{
    return false;
}


bool SqlQuery::next()
{
    return false;
}


QVariant SqlQuery::value(int a_index) const
{
    static_cast<void>(a_index);
    return QVariant();
}


int SqlQuery::numRowsAffected() const
{
    return 0;
}


}  // namespace qtutils {


#endif  //  #ifdef QTUTILS_USE_NON_STANDARD_DB
