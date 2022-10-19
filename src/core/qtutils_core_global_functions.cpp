//
// file:            qtutils_core_global_functions.hpp
// path:			src/core/qtutils_core_global_functions.hpp
// created on:		2022 Oct 19
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/core/global_functions.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QJsonDocument>


namespace qtutils { namespace core{


static inline QJsonObject ParseJWTPart(const QByteArray& a_inpArray)
{
    const QByteArray inpPayload = QByteArray::fromBase64( a_inpArray );
    const QJsonDocument inpPayloadJDoc = QJsonDocument::fromJson( inpPayload );
    const QJsonObject inpPayloadJObj = inpPayloadJDoc.object();
    return inpPayloadJObj;
}


QTUTILS_EXPORT QList<QJsonObject> ParseJWT(const QByteArray& a_inpBA)
{
    const QList<QByteArray> inpList = a_inpBA.split('.');

    if(inpList.size()!=3){
        return QList<QJsonObject>();
    }

    QList<QJsonObject> retList;
    for(int i(0); i<3; ++i){
        retList.push_back(ParseJWTPart(inpList.at(i)));
    }

    return retList;
}


}}  // namespace qtutils { namespace core{
