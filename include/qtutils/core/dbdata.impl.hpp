//
// file:            dbdata.impl.hpp
// path:			include/qtutils/tools/dbdata.impl.hpp
// created on:		2022 Oct 11
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_TOOLS_DBDATA_IMPL_HPP
#define QTUTILS_INCLUDE_QTUTILS_TOOLS_DBDATA_IMPL_HPP

#ifndef QTUTILS_INCLUDE_QTUTILS_TOOLS_DBDATA_HPP
#include "dbdata.hpp"
#endif

#include <utility>


namespace qtutils{ namespace core{

#ifndef QTUTILS_TOOLS_DBDATA_GETID
#define QTUTILS_TOOLS_DBDATA_GETID  id
#endif


template <typename Type>
SharedPtr<Type>::SharedPtr()
    :
      ::std::shared_ptr<Type>(new Type())
{
}


template <typename Type>
SharedPtr<Type>::SharedPtr(Type* a_ptr)
    :
      ::std::shared_ptr<Type>(a_ptr)
{
}


template <typename Type>
bool SharedPtr<Type>::operator!=(const SharedPtr& a_cM)const
{
    const Type*const pThis = ::std::shared_ptr<Type>::get();
    const Type*const pThat = a_cM.get();

    if(pThis==pThat){return false;}
    if((!pThis) || (!pThat)){return true;}
    return (*pThis) != (*pThat);
}


template <typename TypeSharedPtr>
static inline TypeSharedPtr FromQVariant(const QVariant& a_qv, int* a_pnChanges)
{
    const QVariantMap aDtMap = a_qv.toMap();
    typename TypeSharedPtr::element_type* this_p = new typename TypeSharedPtr::element_type();
    int nChanges = this_p->FromVariantMap(aDtMap);
    if(a_pnChanges){
        *a_pnChanges = nChanges;
    }
    return TypeSharedPtr(this_p);
}


template <typename Type>
template<typename... Targs>
SharedPtr<Type> SharedPtr<Type>::fromQVariant(const QVariant& a_qv,Targs... a_args)
{
    const QVariantMap aDtMap = a_qv.toMap();
    Type* this_p = new Type();
    this_p->FromVariantMap(aDtMap,a_args...);
    return SharedPtr(this_p);
}


template <typename Type>
template<typename TypeVM,typename... Targs>
int SharedPtr<Type>::FromVariantMap(const TypeVM& a_vmt,Targs... a_args)
{
    const QVariantMap aDtMap = TypeVmToVariantMap(a_vmt);
    Type* this_p = ::std::shared_ptr<Type>::get();
    assert(this_p);
    return this_p->FromVariantMap(aDtMap,a_args...);
}


template <typename Type, typename ContKey,typename ContType>
GroupOfType<Type,ContKey,ContType>::operator QVariant()const
{
    QVariantList aDtList;
    typename ContType::const_iterator iter = ContType::begin();

    for(;iter!=ContType::s_constNullIter;++iter){
        aDtList.push_back((QVariant)iter->second);
    }

    return QVariant(aDtList);
}


template <typename Type, typename ContKey,typename ContType>
template<typename... Targs>
GroupOfType<Type,ContKey,ContType> GroupOfType<Type,ContKey,ContType>::fromQVariant(const QVariant& a_qv,Targs... a_args)
{
    GroupOfType retCR;
    const QVariantList aList(a_qv.toList());
    QVariant elm;
    foreach(elm, aList){
        try{
            Type cr = Type::fromQVariant(elm,a_args...);
            retCR.AddEntryIfNotExistC(::std::pair<ContKey,Type>(cr->QTUTILS_TOOLS_DBDATA_GETID,cr));
        }
        catch(...){
            //
        }
    }
    //QVariantList::const_reverse_iterator riter = aList.crbegin();
    //const QVariantList::const_reverse_iterator riterEnd = aList.crend();
    //for(;riter!=riterEnd;++riter){
    //    elm = *riter;
    //    try{
    //        Type cr = Type::fromQVariant(elm,a_args...);
    //        retCR.AddEntryIfNotExistC(::std::pair<ContKey,Type>(cr->id,cr));
    //    }
    //    catch(...){
    //        //
    //    }
    //}

    return retCR;
}


template <typename Type, typename ContKey,typename ContType>
bool GroupOfType<Type,ContKey,ContType>::operator!=(const GroupOfType& a_cM)const
{
    const size_t sizeThis = ContType::size();
    const size_t sizeThat = a_cM.size();
    if(sizeThis!=sizeThat){return true;}
    if(!sizeThis){return false;}

    typename ContType::const_iterator thisIter = ContType::begin();
    typename ContType::const_iterator thatIter = a_cM.begin();

    for(;thisIter != ContType::s_constNullIter;++thisIter,++thatIter){
        if((thisIter->second)!=(thatIter->second)){
            return true;
        }
    }  //  for(;thisIter != ContType::s_constNullIter;++thisIter,++thatIter){

    return false;
}


}}  //  namespace qtutils{ namespace core{


#endif  //  #ifndef QTUTILS_INCLUDE_QTUTILS_TOOLS_DBDATA_IMPL_HPP
