//
// file:            qtutils_core_global_functions.hpp
// path:			src/core/qtutils_core_global_functions.hpp
// created on:		2022 Oct 19
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/core/global_functions.hpp>
#include <qtutils/version_resolving.h>
#include <stdio.h>
#include <qtutils/disable_utils_warnings.h>
#include <QJsonDocument>
#include <QStringList>
#include <QJsonArray>

#ifdef snprintf_t
#undef snprintf_t
#endif
#ifdef _MSC_VER
#define snprintf_t(_buf,_count,...)     _snprintf_s(_buf,_count,_count,__VA_ARGS__)
#else
#define snprintf_t  snprintf
#endif


namespace qtutils { namespace core{


static void SerializeJsonArrayStatic(const QJsonArray& a_array, const JsonValueSerializer& a_serializer,bool a_srlzType);
static void SerializeJsonObjectStatic(const QJsonObject& a_obj, const JsonValueSerializer& a_serializer,bool a_srlzType, const char* a_cpcPrepend, const char* a_cpcDelimer);

static inline void SerializeJsonValueInline(const QJsonValue& a_val, const JsonValueSerializer& a_serializer,bool a_srlzType){
    const QJsonValue::Type type = a_val.type();
    switch(type){
    case QJsonValue::Null:
        a_serializer("Null");
        break;
    case QJsonValue::Bool:
        if(a_srlzType){a_serializer("B:");}
        a_serializer(a_val.toBool()?"true":"false");
        break;
    case QJsonValue::Double:{
        char vcBuffer[512];
        if(a_srlzType){a_serializer("N:");}
        snprintf_t(vcBuffer,511,"%.10g",a_val.toDouble());
        a_serializer(vcBuffer);
    }break;
    case QJsonValue::String:
        if(a_srlzType){a_serializer("S:");}
        a_serializer("\"");
        a_serializer(a_val.toString().toStdString().c_str());
        a_serializer("\"");
        break;
    case QJsonValue::Array:
        if(a_srlzType){a_serializer("A:");}
        SerializeJsonArrayStatic(a_val.toArray(),a_serializer,a_srlzType);
        break;
    case QJsonValue::Object:
        if(a_srlzType){a_serializer("O:");}
        a_serializer("{");
        SerializeJsonObjectStatic(a_val.toObject(),a_serializer,a_srlzType,"",",");
        a_serializer("}"); 
        break;
    default:
        a_serializer("UnknownType");
        break;
    }  //  switch(type){
}


QTUTILS_EXPORT void SerializeJsonValue(const QJsonValue& a_val, const JsonValueSerializer& a_serializer,bool a_srlzType)
{
    SerializeJsonValueInline(a_val,a_serializer,a_srlzType);
}


QTUTILS_EXPORT void SerializeJsonObject(const QJsonObject& a_obj, const JsonValueSerializer& a_serializer,bool a_srlzType, const char* a_cpcPrepend, const char* a_cpcDelimer)
{
    SerializeJsonObjectStatic(a_obj,a_serializer,a_srlzType,a_cpcPrepend,a_cpcDelimer);
}


static void SerializeJsonArrayStatic(const QJsonArray& a_array, const JsonValueSerializer& a_serializer,bool a_srlzType)
{
    const sizetype count = a_array.size();
    a_serializer("[");
    if(count>0){
        SerializeJsonValueInline(a_array[0],a_serializer,a_srlzType);
        for(sizetype i(1); i<count;++i){
            a_serializer(",");
            SerializeJsonValueInline(a_array[i],a_serializer,a_srlzType);
        }
    }
    a_serializer("]");
}


static void SerializeJsonObjectStatic(const QJsonObject& a_obj, const JsonValueSerializer& a_serializer,bool a_srlzType, const char* a_cpcPrepend, const char* a_cpcDelimer)
{
    const QStringList keys = a_obj.keys();
    sizetype count = keys.size();
    if(count>0){
        --count;
        for(sizetype i(0); i<count;++i){
            const QString& key = keys[i];
            a_serializer(a_cpcPrepend);
            a_serializer(key.toStdString().c_str());
            a_serializer(":");
            SerializeJsonValueInline(a_obj.value(key),a_serializer,a_srlzType);
            a_serializer(a_cpcDelimer);
        }
        const QString& key = keys[count];
        a_serializer(a_cpcPrepend);
        a_serializer(key.toStdString().c_str());
        a_serializer(":");
        SerializeJsonValueInline(a_obj.value(key),a_serializer,a_srlzType);
    }
}


#ifdef QTUITILS_PARSE_JWT_IN_GLOBAL

static inline QJsonObject ParseJWTPart(const QByteArray& a_inpArray)
{
    const QByteArray inpPayload = QByteArray::fromBase64( a_inpArray );
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
    if(a_pSignatureBuff){*a_pSignatureBuff=inpList.at(2);}

    return retList;
}


#endif  //  #ifdef QTUITILS_PARSE_JWT_IN_GLOBAL


}}  // namespace qtutils { namespace core{
