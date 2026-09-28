//
// file:            global_functions.hpp
// path:			include/qtutils/core/global_functions.hpp
// created on:		2022 Jan 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#pragma once


#include <qtutils/export_symbols.h>
#include <cinternal/disable_compiler_warnings.h>
#include <functional>
#include <qtutils/disable_utils_warnings.h>
#include <QList>
#include <QJsonObject>
#include <QJsonValue>
#include <QByteArray>
#include <QString>
#include <cinternal/undisable_compiler_warnings.h>


namespace qtutils { namespace core{


typedef ::std::function<void(const char* a_str)> JsonValueSerializer;

QTUTILS_EXPORT void SerializeJsonValue(const QJsonValue& a_val, const JsonValueSerializer& a_serializer, bool a_srlzType);
QTUTILS_EXPORT void SerializeJsonObject(const QJsonObject& a_obj, const JsonValueSerializer& a_serializer,bool a_srlzType, const char* a_cpcPrepend="", const char* a_cpcDelimer="\n");


}}  // namespace qtutils { namespace core{
