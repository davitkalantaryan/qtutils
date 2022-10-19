//
// file:            global_functions.hpp
// path:			include/qtutils/core/global_functions.hpp
// created on:		2022 Jan 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once


#include <qtutils/export_symbols.h>
#include <qtutils/disable_utils_warnings.h>
#include <QList>
#include <QJsonObject>
#include <QByteArray>


namespace qtutils { namespace core{

QTUTILS_EXPORT QList<QJsonObject> ParseJWT(const QByteArray& a_inpBA);

}}  // namespace qtutils { namespace core{
