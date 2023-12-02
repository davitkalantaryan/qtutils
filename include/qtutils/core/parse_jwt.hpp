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
#include <QMessageAuthenticationCode>


namespace qtutils { namespace core{


QTUTILS_EXPORT QList<QJsonObject> ParseJWT(const QByteArray& a_inpBA, QByteArray* a_pSignatureBuff);
QTUTILS_EXPORT QList<QJsonObject> ParseJWT02(const QByteArray& a_inpBA, QList<QByteArray>* CPPUTILS_ARG_NN a_inpList_p);
QTUTILS_EXPORT QByteArray CalculateJwtSignaturePrs(const QByteArray& a_headerAndPayloadBase64,const QByteArray& a_secret, const QCryptographicHash::Algorithm& a_algEnm);


}}  // namespace qtutils { namespace core{
