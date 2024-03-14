//
// file:            qtutils_core_parse_jwt.hpp
// path:			src/core/qtutils_core_parse_jwt.hpp
// created on:		2023 May 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/core/parse_jwt.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QJsonDocument>


namespace qtutils { namespace core{


static inline QJsonObject ParseJWTPartInline(const QByteArray& a_inpArray){
    const QByteArray inpPayload = QByteArray::fromBase64( a_inpArray, QByteArray::Base64UrlEncoding );
    const QJsonDocument inpPayloadJDoc = QJsonDocument::fromJson( inpPayload );
    const QJsonObject inpPayloadJObj = inpPayloadJDoc.object();
    return inpPayloadJObj;
}


static inline JwtParseResult VerifyJWTInline(const QByteArray& a_jwt, const QByteArray& a_key, QList<QByteArray>* CPPUTILS_ARG_NN a_parts_p){
    *a_parts_p = a_jwt.split('.');
    if (a_parts_p->size() < 3) {
        a_parts_p->resize(3);
        return JwtParseResult::wrong_data;
    }

    // Prepare data to verify (header.payload)
    const QByteArray dataToVerify = (*a_parts_p)[0] + "." + (*a_parts_p)[1];

    // Decode signature from base64-url
    const QByteArray signature = QByteArray::fromBase64((*a_parts_p)[2], QByteArray::Base64UrlEncoding);

    // Use HMAC SHA-256 to compute the signature
    const QByteArray computedSignature = QMessageAuthenticationCode::hash(dataToVerify, a_key, QCryptographicHash::Sha256);

    // Compare the computed signature with the token's signature
    return (signature == computedSignature) ? JwtParseResult::ok : JwtParseResult::wrong_signature;
}


static inline QList<QJsonObject> ParseJWTInline(const QByteArray& a_inpBA, QList<QByteArray>* CPPUTILS_ARG_NN a_inpList_p)
{
    *a_inpList_p = a_inpBA.split('.');

    if(a_inpList_p->size()!=3){
        return QList<QJsonObject>();
    }

    QList<QJsonObject> retList;
    retList.push_back(ParseJWTPartInline(a_inpList_p->at(0)));
    retList.push_back(ParseJWTPartInline(a_inpList_p->at(1)));
    
    return retList;
}


QTUTILS_EXPORT QList<QJsonObject> ParseJWT01(const QByteArray& a_inpBA)
{
    QList<QByteArray> inpList;
    const QList<QJsonObject> retList = ParseJWTInline(a_inpBA,&inpList);
    
    return retList;
}


QTUTILS_EXPORT QList<QJsonObject> ParseJWT03(const QByteArray& a_inpBA, QList<QByteArray>* CPPUTILS_ARG_NN a_inpList_p)
{
    a_inpList_p->clear();
    return ParseJWTInline(a_inpBA,a_inpList_p);
}


QTUTILS_EXPORT JwtParseResult VerifyJWT01(const QByteArray& a_jwt, const QByteArray& a_key) 
{
    QList<QByteArray> parts;
    return VerifyJWTInline(a_jwt,a_key,&parts);
}


QTUTILS_EXPORT JwtParseResult VerifyJWT02(const QByteArray& a_jwt, const QByteArray& a_key, QList<QByteArray>* CPPUTILS_ARG_NN a_parts_p) 
{
    return VerifyJWTInline(a_jwt,a_key,a_parts_p);
}


QTUTILS_EXPORT JwtParseResult VerifyJWTAndParse(const QByteArray& a_jwt, const QByteArray& a_key, QList<QJsonObject>* CPPUTILS_ARG_NN a_inpList_p) 
{
    QList<QByteArray> parts;
    const JwtParseResult cbRet = VerifyJWTInline(a_jwt,a_key,&parts);
    a_inpList_p->push_back(ParseJWTPartInline(parts[0]));
    a_inpList_p->push_back(ParseJWTPartInline(parts[1]));
    return cbRet;
}



}}  // namespace qtutils { namespace core{
