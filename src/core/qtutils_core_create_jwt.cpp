//
// file:            qtutils_core_parse_jwt.hpp
// path:			src/core/qtutils_core_parse_jwt.hpp
// created on:		2023 May 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/core/global_functions.hpp>
#include <qtutils/version_resolving.h>
#include <stdlib.h>
#include <time.h>
#include <qtutils/disable_utils_warnings.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCryptographicHash>
#include <QMessageAuthenticationCode>

#define QTUTILS_JWT_SECR01_LEN      512


namespace qtutils { namespace core{


QTUTILS_EXPORT QByteArray GenerateJwtSecret01(void)
{
    QByteArray retByte;
    retByte.resize(QTUTILS_JWT_SECR01_LEN);
    for(sizetype i(0);i<QTUTILS_JWT_SECR01_LEN;++i){
        retByte[i] = static_cast<char>(rand());
    }
    return retByte;
}


QTUTILS_EXPORT QByteArray CreateJWT(const QString& a_alg, const QVariantMap& a_payloadVM, const QByteArray& a_secret, QByteArray* a_pSignatureBase64)
{
    const sizetype algSize = a_alg.size();
    if(algSize<5){
        return QByteArray();
    }
    QCryptographicHash::Algorithm algEnm;
    const QString algNumStr = a_alg.mid(2);
    const int algNum = algNumStr.toInt();
    switch(algNum){
    case 256:
        algEnm = QCryptographicHash::Sha256;
        break;
    case 384:
        algEnm = QCryptographicHash::Sha384;
        break;
    case 512:
        algEnm = QCryptographicHash::Sha512;
        break;
    default:
        return QByteArray();
    }  //  switch(algNum){
    
    const QJsonDocument payloadJsonDoc = QJsonDocument(QJsonObject::fromVariantMap(a_payloadVM));
    const QByteArray payloadBA = payloadJsonDoc.toJson(QJsonDocument::Compact);
    const QByteArray payloadBABase64 = payloadBA.toBase64();
    const QVariantMap headerVM = {
        {"alg", a_alg},
        {"typ", QString("JWT")},
    };
    const QJsonDocument headerJsonDoc = QJsonDocument(QJsonObject::fromVariantMap(headerVM));
    const QByteArray headerBA = headerJsonDoc.toJson(QJsonDocument::Compact);
    const QByteArray headerBABase64 = headerBA.toBase64();
    const QByteArray headerAndPayload = headerBABase64 + "." + payloadBABase64;
    const QByteArray signatureBA = QMessageAuthenticationCode::hash(headerAndPayload, a_secret, algEnm);
    if(a_pSignatureBase64){
        *a_pSignatureBase64 = signatureBA.toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
        return headerAndPayload + "." + (*a_pSignatureBase64);
    }
    return headerAndPayload + "." + signatureBA.toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
}


class CPPUTILS_DLL_PRIVATE ClassSRandIniter{
public:
    ClassSRandIniter(){
        time_t currentTime;
        currentTime = time(&currentTime);
        srand(static_cast<int>(currentTime));
    }
}static s_srandIniter;


}}  // namespace qtutils { namespace core{
