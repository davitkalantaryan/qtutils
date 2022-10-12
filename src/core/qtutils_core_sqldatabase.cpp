//
// file:            qtutils_core_sqldatabase.cpp
// path:			src/core/qtutils_core_sqldatabase.cpp
// created on:		2022 Oct 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/core/sqldatabase.hpp>

#ifdef QTUTILS_USE_NON_STANDARD_DB

#include <cpputils/flagshelper.h>

namespace qtutils {

class CPPUTILS_DLL_PRIVATE SqlDatabase_p
{
public:
    CPPUTILS_FLAGS_UN(
            isOpen
    )m_flags;

};



const char* SqlDatabase::defaultConnection = "";


SqlDatabase::~SqlDatabase()
{
    delete m_db_data_p;
}



SqlDatabase::SqlDatabase()
    :
      m_db_data_p(new SqlDatabase_p())
{
    m_db_data_p->m_flags.all = CPPUTILS_INIT_BITS;
}


SqlDatabase::SqlDatabase(const SqlDatabase& a_cM)
    :
      m_db_data_p(new SqlDatabase_p(*a_cM.m_db_data_p))
{
}


SqlDatabase::SqlDatabase(SqlDatabase&& a_mM)
    :
      m_db_data_p(a_mM.m_db_data_p)
{
    a_mM.m_db_data_p = nullptr;
}


SqlDatabase& SqlDatabase::operator=(const SqlDatabase& a_cM)
{
    *m_db_data_p = *(a_cM.m_db_data_p);
    return *this;

}


SqlDatabase& SqlDatabase::operator=(SqlDatabase&& a_mM)
{
    SqlDatabase_p*const this_dt = m_db_data_p;
    m_db_data_p = a_mM.m_db_data_p;
    a_mM.m_db_data_p = this_dt;
    return *this;
}


bool SqlDatabase::isOpen() const
{
    return m_db_data_p->m_flags.b.isOpen_true?true:false;
}


bool SqlDatabase::open()
{
    m_db_data_p->m_flags.b2.isOpen_both = CPPUTILS_MAKE_BITS_POSITIVE;
    return true;
}


void SqlDatabase::close()
{
    m_db_data_p->m_flags.b2.isOpen_both = CPPUTILS_MAKE_BITS_NEGATIVE;
}


void SqlDatabase::setDatabaseName(const QString& a_name)
{
    static_cast<void>(a_name);
}


SqlError SqlDatabase::lastError() const
{
    return SqlError();
}


bool SqlDatabase::commit()
{
    return true;
}


SqlDatabase	SqlDatabase::addDatabase(const QString& a_type, const QString& a_connectionName)
{
    static_cast<void>(a_type);
    static_cast<void>(a_connectionName);
    return SqlDatabase();
}


void SqlDatabase::removeDatabase(const QString& a_connectionName)
{
    static_cast<void>(a_connectionName);
}


}  // namespace qtutils {


#endif  //  #ifdef QTUTILS_USE_NON_STANDARD_DB
