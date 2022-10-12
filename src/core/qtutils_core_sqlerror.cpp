//
// file:            qtutils_core_sqlerror.cpp
// path:			src/core/qtutils_core_sqlerror.cpp
// created on:		2022 Oct 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/core/sqlerror.hpp>

#ifdef QTUTILS_USE_NON_STANDARD_DB

namespace qtutils {

class CPPUTILS_DLL_PRIVATE SqlError_p
{
    //
};


SqlError::~SqlError()
{
    delete m_err_data_p;
}


SqlError::SqlError()
    :
      m_err_data_p(new SqlError_p())
{
}


QString SqlError::databaseText() const
{
    return "";
}


QString SqlError::driverText() const
{
    return "";
}


QString SqlError::nativeErrorCode() const
{
    return "";
}


QString SqlError::text() const
{
    return "";
}


SqlError::ErrorType SqlError::type() const
{
    return UnknownError;
}


}  // namespace qtutils {


#endif  //  #ifdef QTUTILS_USE_NON_STANDARD_DB
