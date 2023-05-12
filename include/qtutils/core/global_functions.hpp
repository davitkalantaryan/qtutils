//
// file:            global_functions.hpp
// path:			include/qtutils/core/global_functions.hpp
// created on:		2022 Jan 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once


#include <qtutils/export_symbols.h>
#include <functional>
#include <qtutils/disable_utils_warnings.h>
#include <QList>
#include <QJsonObject>
#include <QJsonValue>
#include <QByteArray>
#include <QString>


namespace qtutils { namespace core{


typedef ::std::function<void(const char* a_str)> JsonValueSerializer;

QTUTILS_EXPORT void SerializeJsonValue(const QJsonValue& a_val, const JsonValueSerializer& a_serializer, bool a_srlzType);
QTUTILS_EXPORT void SerializeJsonObject(const QJsonObject& a_obj, const JsonValueSerializer& a_serializer,bool a_srlzType, const char* a_cpcPrepend="", const char* a_cpcDelimer="\n");


#ifdef QTUITILS_PARSE_JWT_IN_GLOBAL

//  see: include/qtutils/core/parse_jwt.hpp

QTUTILS_EXPORT QList<QJsonObject> ParseJWT(const QByteArray& a_inpBA, QByteArray* a_pSignatureBuff);

#endif  //  #ifdef QTUITILS_PARSE_JWT_IN_GLOBAL


}}  // namespace qtutils { namespace core{
