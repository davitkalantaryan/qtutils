//
// file:            parse_jwt.hpp
// path:			include/qtutils/core/parse_jwt.hpp
// created on:		2023 May 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once


#include <qtutils/export_symbols.h>
#include <qtutils/disable_utils_warnings.h>
#include <QList>
#include <QJsonObject>
#include <QByteArray>


namespace qtutils { namespace core{


QTUTILS_EXPORT QList<QJsonObject> ParseJWT(const QByteArray& a_inpBA, QByteArray* a_pSignatureBuff);


}}  // namespace qtutils { namespace core{
