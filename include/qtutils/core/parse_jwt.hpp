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


enum class JwtParseResult{
    ok,
    expired,
    wrong_signature,
    wrong_data,
    unknown
};

QTUTILS_EXPORT QList<QJsonObject> ParseJWT01(const QByteArray& a_inpBA);
QTUTILS_EXPORT QList<QJsonObject> ParseJWT03(const QByteArray& a_inpBA, QList<QByteArray>* CPPUTILS_ARG_NN a_inpList_p);
QTUTILS_EXPORT JwtParseResult VerifyJWT01(const QByteArray& a_jwt, const QByteArray& a_key);
QTUTILS_EXPORT JwtParseResult VerifyJWT02(const QByteArray& a_jwt, const QByteArray& a_key, QList<QByteArray>* CPPUTILS_ARG_NN a_parts_p);
QTUTILS_EXPORT JwtParseResult VerifyJWTAndParse(const QByteArray& a_jwt, const QByteArray& a_key, QList<QJsonObject>* CPPUTILS_ARG_NN a_inpList_p);


}}  // namespace qtutils { namespace core{
