//
// repo:            qtutils
// file:            qtkeychain.hpp
// path:			include/qtutils/core/qtkeychain.hpp
// created on:		2026 Aug 29
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#pragma once


#include <qtutils/export_symbols.h>
#include <cinternal/disable_compiler_warnings.h>
#include <qtutils/disable_utils_warnings.h>
#include <QString>
#include <QByteArray>
#include <cinternal/undisable_compiler_warnings.h>


namespace qtutils{ namespace core{ namespace qtkeychain{


QTUTILS_EXPORT bool SaveData(const QString& a_service,const QString& a_key,const QByteArray& a_data);
QTUTILS_EXPORT QByteArray RetriveData(const QString& a_service,const QString& a_key);
QTUTILS_EXPORT void DeleteData(const QString& a_service, const QString& a_key);
QTUTILS_EXPORT void DeleteAllKeys(const QString& a_service);


}}}  //  namespace qtutils{ namespace core{ namespace qtkeychain{
