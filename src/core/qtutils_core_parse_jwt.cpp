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


static inline QByteArray CalculateJwtSignaturePrsInline(const QByteArray& a_headerAndPayloadBase64,const QByteArray& a_secret, const QCryptographicHash::Algorithm& a_algEnm){
    const QByteArray signatureBA = QMessageAuthenticationCode::hash(a_headerAndPayloadBase64, a_secret, a_algEnm);
    return signatureBA.toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
}


static inline QJsonObject ParseJWTPartInline(const QByteArray& a_inpArray){
    const QByteArray inpPayload = QByteArray::fromBase64( a_inpArray, QByteArray::Base64UrlEncoding );
    const QJsonDocument inpPayloadJDoc = QJsonDocument::fromJson( inpPayload );
    const QJsonObject inpPayloadJObj = inpPayloadJDoc.object();
    return inpPayloadJObj;
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


QTUTILS_EXPORT QList<QJsonObject> ParseJWT(const QByteArray& a_inpBA, QByteArray* a_pSignatureBuff)
{
    QList<QByteArray> inpList;
    const QList<QJsonObject> retList = ParseJWTInline(a_inpBA,&inpList);
    
    if((retList.size()>0)&&a_pSignatureBuff){
        *a_pSignatureBuff=inpList.at(2);
    }

    return retList;
}


QTUTILS_EXPORT QList<QJsonObject> ParseJWT02(const QByteArray& a_inpBA, QList<QByteArray>* CPPUTILS_ARG_NN a_inpList_p)
{
    a_inpList_p->clear();
    return ParseJWTInline(a_inpBA,a_inpList_p);    
}


QTUTILS_EXPORT QByteArray CalculateJwtSignaturePrs(const QByteArray& a_headerAndPayloadBase64,const QByteArray& a_secret, const QCryptographicHash::Algorithm& a_algEnm)
{
    return CalculateJwtSignaturePrsInline(a_headerAndPayloadBase64,a_secret, a_algEnm);
}


}}  // namespace qtutils { namespace core{
