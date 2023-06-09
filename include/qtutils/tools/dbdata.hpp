//
// file:            dbdata.hpp
// path:			include/qtutils/tools/dbdata.hpp
// created on:		2022 Oct 11
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_TOOLS_DBDATA_HPP
#define QTUTILS_INCLUDE_QTUTILS_TOOLS_DBDATA_HPP

#include <qtutils/export_symbols.h>
#include <cpputils/hash/vhash.hpp>
#include <memory>
#include <qtutils/disable_utils_warnings.h>
#include <QVariantMap>
#include <QJsonObject>
#include <QVariant>
#include <QByteArray>


namespace qtutils{ namespace tools{


QTUTILS_EXPORT QVariantMap  TypeVmToVariantMap(const QByteArray& a_ba);
QTUTILS_EXPORT QVariantMap  TypeVmToVariantMap(const QJsonObject& a_jo);
QTUTILS_EXPORT QVariantMap  TypeVmToVariantMap(const QVariant& a_qv);
QTUTILS_EXPORT const QVariantMap&  TypeVmToVariantMap(const QVariantMap& a_vm);


template <typename Type>
class SharedPtr : public ::std::shared_ptr<Type>
{
public:
    SharedPtr();
    SharedPtr(Type* a_ptr);
    bool operator!=(const SharedPtr& a_cM)const;
    template<typename... Targs>
    static SharedPtr<Type> fromQVariant(const QVariant& a_qv,Targs... a_args);
    template<typename TypeVM,typename... Targs>
    int FromVariantMap(const TypeVM& a_vmt,Targs... a_args);
};


template <typename Type, typename ContKey=int,typename ContType=::cpputils::hash::VHash<ContKey,Type> >
class GroupOfType : public ContType
{
public:
    //template<typename... Targs>
    //GroupOfType(Targs... a_args);

    GroupOfType()=default;
    GroupOfType(const GroupOfType&)=default;
    GroupOfType(GroupOfType&&)=default;

    GroupOfType& operator=(const GroupOfType& a_cM);
    GroupOfType& operator=(GroupOfType&& a_mM);

    operator QVariant()const;
    template<typename... Targs>
    static GroupOfType fromQVariant(const QVariant& a_qv,Targs... a_args);

    void TakeFromOtherContainer(GroupOfType& a_mM);
    bool operator!=(const GroupOfType& a_cM)const;
};


template <typename TypeSharedPtr>
static inline TypeSharedPtr FromQVariant(const QVariant& a_qv, int* a_pnChanges=nullptr);

#define QTUTILS_TOOLS_CAST_OPERS(_FinalType,_SourceType)        \
    public:                                                     \
    _FinalType(const _SourceType& a_cM) : _SourceType(a_cM){}   \
    _FinalType(_SourceType&& a_mM) : _SourceType(a_mM){}        \
    _FinalType()=default;                                       \
    _FinalType& operator=(const _SourceType& a_cM){             \
        _SourceType::operator=(a_cM);                           \
        return *this;                                           \
    }                                                           \
    _FinalType& operator=(_SourceType&& a_mM){                  \
        _SourceType::operator= (::std::move(a_mM));             \
        return *this;                                           \
    }

#define QTUTILS_TOOLS_FIND_FIELD_RAW(_nChanges,_variantMap,_field,_keyName,_fieldType,_cont)   \
    if((_variantMap).contains(_keyName)){                                       \
        _fieldType newVal((_variantMap).value(_keyName).value<_fieldType>());   \
        if(newVal!=(_cont)._field){                                               \
            (_cont)._field = (::std::move(newVal));                               \
            ++(_nChanges);                                                      \
        }                                                                       \
    }

#define QTUTILS_TOOLS_FIND_FIELD02(_vm,_cont,_field)    \
    QTUTILS_TOOLS_FIND_FIELD_RAW(nChanges,(_vm),_field,#_field,decltype((_cont)._field),_cont)

#define QTUTILS_TOOLS_FIND_FIELD(_field)    QTUTILS_TOOLS_FIND_FIELD02(a_variantMap,*this,_field)


}}  //  namespace qtutils{ namespace tools{


#ifndef QTUTILS_INCLUDE_QTUTILS_TOOLS_DBDATA_IMPL_HPP
#include "dbdata.impl.hpp"
#endif

#endif  //  #ifndef QTUTILS_INCLUDE_QTUTILS_TOOLS_DBDATA_HPP
