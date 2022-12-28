//
// file:            qtutils_tools_dbdata.cpp
// path:			src/tools/qtutils_tools_dbdata.cpp
// created on:		2022 Oct 11
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/tools/dbdata.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QJsonDocument>


namespace qtutils{ namespace tools{


QTUTILS_EXPORT QVariantMap TypeVmToVariantMap(const QByteArray& a_ba)
{
    const QJsonDocument aJsonDoc = QJsonDocument::fromJson(a_ba);
    const QJsonObject aJsonObj = aJsonDoc.object();
    return aJsonObj.toVariantMap();
}


QTUTILS_EXPORT QVariantMap TypeVmToVariantMap(const QJsonObject& a_jo)
{
    return a_jo.toVariantMap();
}


QTUTILS_EXPORT QVariantMap TypeVmToVariantMap(const QVariant& a_qv)
{
    return a_qv.toMap();
}


QTUTILS_EXPORT const QVariantMap& TypeVmToVariantMap(const QVariantMap& a_vm)
{
    return a_vm;
}


}}  //  namespace qtutils{ namespace tools{
