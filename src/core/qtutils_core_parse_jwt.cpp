//
// file:            qtutils_core_parse_jwt.hpp
// path:			src/core/qtutils_core_parse_jwt.hpp
// created on:		2023 May 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/core/global_functions.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QJsonDocument>


namespace qtutils { namespace core{


static inline QJsonObject ParseJWTPart(const QByteArray& a_inpArray)
{
    const QByteArray inpPayload = QByteArray::fromBase64( a_inpArray, QByteArray::Base64UrlEncoding );
    const QJsonDocument inpPayloadJDoc = QJsonDocument::fromJson( inpPayload );
    const QJsonObject inpPayloadJObj = inpPayloadJDoc.object();
    return inpPayloadJObj;
}


QTUTILS_EXPORT QList<QJsonObject> ParseJWT(const QByteArray& a_inpBA, QByteArray* a_pSignatureBuff)
{
    const QList<QByteArray> inpList = a_inpBA.split('.');

    if(inpList.size()!=3){
        return QList<QJsonObject>();
    }

    QList<QJsonObject> retList;
    retList.push_back(ParseJWTPart(inpList.at(0)));
    retList.push_back(ParseJWTPart(inpList.at(1)));
    if(a_pSignatureBuff){
        *a_pSignatureBuff=inpList.at(2);
    }

    return retList;
}


}}  // namespace qtutils { namespace core{
