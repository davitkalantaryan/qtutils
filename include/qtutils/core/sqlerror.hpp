//
// file:                sqlerror.hpp
// path:                include/qtutils/core/sqlerror.hpp
// created on:          2022 Oct 12
// created by:          Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#pragma once

#include <qtutils/export_symbols.h>

#ifdef QTUTILS_USE_NON_STANDARD_DB
#include <qtutils/disable_utils_warnings.h>
#include <QString>
#else
#include <qtutils/disable_utils_warnings.h>
#include <QSqlError>
#endif


namespace qtutils {

#ifdef QTUTILS_USE_NON_STANDARD_DB

class QTUTILS_EXPORT SqlError
{
public:
    enum ErrorType{None,ET1};
public:
    QString     databaseText() const;
    QString     driverText() const;
    QString     nativeErrorCode() const;
    QString     text() const;
    ErrorType   type() const;
};

#else

typedef QSqlError SqlError;

#endif

}  //  namespace qtutils {
