//
// repo:            qtutils
// file:            qtutils_core_global_functions_02.impl.hpp
// path:			include/qtutils/core/impl/qtutils_core_global_functions_02.impl.hpp
// created on:		2026 Aug 29
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//


#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_CORE_IMPL_GLOBAL_FUNCTIONS_02_IMPL_HPP
#define QTUTILS_INCLUDE_QTUTILS_CORE_IMPL_GLOBAL_FUNCTIONS_02_IMPL_HPP


#ifndef QTUTILS_INCLUDE_QTUTILS_CORE_GLOBAL_FUNCTIONS_02_HPP
#include <qtutils/core/global_functions_02.hpp>
#endif

#include <cinternal/disable_compiler_warnings.h>
#include <qtutils/disable_utils_warnings.h>
#include <QDataStream>
#include <QIODevice>
#include <cinternal/undisable_compiler_warnings.h>

namespace qtutils { namespace core{ namespace gf02{


#define QTUTILS_GLOBAL_SERIALIZE_ENTRIES_MAGIC      0x51475345  // "QGSE -> Q(0x51)G(0x47)S(0x53)E(0x45)"
#define QTUTILS_GLOBAL_SERIALIZE_CUR_VERSION        1


template <typename TypeToSrlzDsrlz>
QByteArray Serialize(const TypeToSrlzDsrlz& a_data)
{
    QByteArray dataBS;
    QDataStream stream(&dataBS, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_6_8);
    stream << static_cast<quint32>(QTUTILS_GLOBAL_SERIALIZE_ENTRIES_MAGIC);     // magic
    stream << static_cast<quint32>(QTUTILS_GLOBAL_SERIALIZE_CUR_VERSION);       // format version
    stream << a_data;
    return dataBS;
}


template <typename TypeToSrlzDsrlz>
TypeToSrlzDsrlz Deserialize(const QByteArray& a_dataBS, bool* a_pbOk)
{
    TypeToSrlzDsrlz retData;
    QDataStream stream(a_dataBS);
    stream.setVersion(QDataStream::Qt_6_8);
    quint32 magic;
    stream >> magic;
    if(magic!=static_cast<quint32>(QTUTILS_GLOBAL_SERIALIZE_ENTRIES_MAGIC)){
        // bad magic
        if(a_pbOk){
            *a_pbOk = false;
        }
        return retData;
    }  //  if(magic!=static_cast<quint32>(QTUTILS_GLOBAL_SERIALIZE_ENTRIES_MAGIC)){

    if(a_pbOk){
        *a_pbOk = true;
    }

    quint32 version;
    stream >> version;
    switch(version){
    case 1:
        stream >> retData;
        return retData;
    break;
    default:
        break;
    }  //  switch(version){

    if(a_pbOk){
        *a_pbOk = false;
    }
    return retData;
}


}}}  //  namespace qtutils { namespace core{ namespace gf02{


#endif  //  #ifndef QTUTILS_INCLUDE_QTUTILS_CORE_IMPL_GLOBAL_FUNCTIONS_02_IMPL_HPP
