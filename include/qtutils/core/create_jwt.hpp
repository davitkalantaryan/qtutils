//
// file:            create_jwt.hpp
// path:			include/qtutils/core/create_jwt.hpp
// created on:		2023 May 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once


#include <qtutils/export_symbols.h>
#include <qtutils/disable_utils_warnings.h>
#include <QList>
#include <QString>
#include <QByteArray>
#include <QVariantMap>


namespace qtutils { namespace core{


QTUTILS_EXPORT QByteArray GenerateJwtSecret01(void);
QTUTILS_EXPORT QByteArray CreateJWT(const QString& a_alg, const QVariantMap& a_payload, const QByteArray& a_secret, QByteArray* a_pSignatureBase64);


}}  // namespace qtutils { namespace core{
