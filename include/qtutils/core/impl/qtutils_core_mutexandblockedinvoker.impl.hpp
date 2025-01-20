//
// repo:            qtutils
// file:            qtutils_core_clsinvokeblocked.cpp
// path:			src/core/qtutils_core_clsinvokeblocked.cpp
// created on:		2024 May 15
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_CORE_IMPL_MUTEXANDBLOCKEDINVOKER_IMPL_HPP
#define QTUTILS_INCLUDE_QTUTILS_CORE_IMPL_MUTEXANDBLOCKEDINVOKER_IMPL_HPP


#ifndef QTUTILS_INCLUDE_QTUTILS_CORE_MUTEXANDBLOCKEDINVOKER_HPP
#include <qtutils/core/mutexandblockedinvoker.hpp>
#endif

#include <cinternal/thread_local_sys.h>
#include <tuple>
#include <stddef.h>
#include <qtutils/disable_utils_warnings.h>
#include <QMetaObject>
#include <QThread>

namespace qtutils { namespace core{ namespace mutexandblockedinvoker{


static void PthreadKeyCreateDeleteClbk(void*){}


struct BlockedCallerThreadId{
    BlockedCallerThreadId   *caller;
    ::std::thread::id       m_id;
    bool                    isBlockedCall;
    bool                    reserved01[7];
};


/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

template <typename BaseMutex, typename... Targs>
Mutex<BaseMutex,Targs...>::~Mutex()
{
    CinternalTlsDelete(m_tls_data);
}


template <typename BaseMutex, typename... Targs>
template <typename... TargsMtx>
Mutex<BaseMutex,Targs...>::Mutex(TargsMtx... a_mtxArgs)
    :
      m_mutex(a_mtxArgs...)
{
    CinternalTlsAlloc(&m_tls_data,&PthreadKeyCreateDeleteClbk);
    if(m_tls_data==CINTERNAL_TLS_OUT_OF_INDEXES){
        throw "Failure on constructor. Unable to create TLS";
    }
}


template <typename BaseMutex, typename... Targs>
void Mutex<BaseMutex,Targs...>::lock(Targs... a_args)
{
    BlockedCallerThreadId* const pThreadData = (BlockedCallerThreadId*)CinternalTlsGetSpecific(m_tls_data);
    if(pThreadData){
        const ::std::thread::id lockerId = m_mutex.get_locker_thread_id(a_args...);
        if(lockerId!= (::std::thread::id())){
            BlockedCallerThreadId* pStackThreadData = pThreadData;
            while(pStackThreadData){
                if(pStackThreadData->m_id == lockerId){
                    // no need to lock to prevent deadlock
                    m_mutex.increment_lock_count(a_args...);
                    return;
                }  //  if(pStackThreadData->m_id == lockerId){
                pStackThreadData = pStackThreadData->caller;
            }  //  while(pCurThreadData){
        }  //  if(lockerId!= (::std::thread::id())){
    }  //  if(pThreadDataCaller){
    
    // if we are here then we have to lock
    m_mutex.lock(a_args...);
    m_mutex.set_locker_thread_id( ::std::this_thread::get_id(), a_args... );
    m_mutex.increment_lock_count(a_args...);
}


template <typename BaseMutex, typename... Targs>
void Mutex<BaseMutex,Targs...>::unlock(Targs... a_args)
{
    const ::std::thread::id thisThreadId = ::std::this_thread::get_id();
    const ::std::thread::id lockerThreadId = m_mutex.get_locker_thread_id(a_args...);
    const ptrdiff_t currentLocksCount = m_mutex.decrement_lock_count_and_return(a_args...);
    
    if((lockerThreadId==thisThreadId) && (currentLocksCount<1)){
        m_mutex.unlock(a_args...);
    }
}



/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

template <typename BaseMutex>
template <typename... TargsMtx>
SimpleBaseMutex<BaseMutex>::SimpleBaseMutex(TargsMtx... a_mtxArgs)
    :
      BaseMutex(a_mtxArgs...)
{
    m_lockCount = 0;
}


template <typename BaseMutex>
void SimpleBaseMutex<BaseMutex>::lock()
{
    BaseMutex::lock();
}


template <typename BaseMutex>
void SimpleBaseMutex<BaseMutex>::unlock()
{
    BaseMutex::unlock();
}


template <typename BaseMutex>
void SimpleBaseMutex<BaseMutex>::set_locker_thread_id(const ::std::thread::id& a_id)
{
    m_threadId = a_id;
}


template <typename BaseMutex>
::std::thread::id SimpleBaseMutex<BaseMutex>::get_locker_thread_id()const
{
    return m_threadId;
}


template <typename BaseMutex>
void SimpleBaseMutex<BaseMutex>::increment_lock_count()
{
    ++m_lockCount;
}


template <typename BaseMutex>
ptrdiff_t SimpleBaseMutex<BaseMutex>::decrement_lock_count_and_return()
{
    return (--m_lockCount);
}


/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

//template <typename BaseMutex>
//template <typename... TargsMtx>
//ArrayBaseMutex_p<BaseMutex>::ArrayBaseMutex_p(TargsMtx... a_mtxArgs)
//    :
//      m_mutex_p( new BaseMutex(a_mtxArgs...) )
//{
//}


/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////*/


template <typename BaseMutex>
ArrayBaseMutex<BaseMutex>::~ArrayBaseMutex()
{
    delete [] m_pLockData;
}


template <typename BaseMutex>
template <typename... TargsMtx>
ArrayBaseMutex<BaseMutex>::ArrayBaseMutex(TargsMtx... a_mtxArgs)
    :
      BaseMutex( a_mtxArgs... )
{
    const size_t cunCount = static_cast<size_t>( BaseMutex::size() );
    m_pLockData = new SItem[cunCount];
    for(size_t i(0); i<cunCount;++i){
        m_pLockData[i].lockCount = 0;
    }
}


template <typename BaseMutex>
void ArrayBaseMutex<BaseMutex>::lock(const IntType& a_index)
{
    BaseMutex::lock(a_index);
}


template <typename BaseMutex>
void ArrayBaseMutex<BaseMutex>::unlock(const IntType& a_index)
{
    BaseMutex::unlock(a_index);
}


template <typename BaseMutex>
void ArrayBaseMutex<BaseMutex>::set_locker_thread_id(const ::std::thread::id& a_id, const IntType& a_index)
{
    m_pLockData[static_cast<size_t>(a_index)].threadId = a_id;
}


template <typename BaseMutex>
::std::thread::id ArrayBaseMutex<BaseMutex>::get_locker_thread_id(const IntType& a_index)const
{
    return m_pLockData[static_cast<size_t>(a_index)].threadId;
}


template <typename BaseMutex>
void ArrayBaseMutex<BaseMutex>::increment_lock_count(const IntType& a_index)
{
    ++(m_pLockData[static_cast<size_t>(a_index)].lockCount);
}


template <typename BaseMutex>
ptrdiff_t ArrayBaseMutex<BaseMutex>::decrement_lock_count_and_return(const IntType& a_index)
{
    --(m_pLockData[static_cast<size_t>(a_index)].lockCount);
    return static_cast<ptrdiff_t>(m_pLockData[static_cast<size_t>(a_index)].lockCount);
}


/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

template <typename BaseMutex,typename... Targs>
static inline void InvokeBlocked(Mutex<BaseMutex,Targs...>* CPPUTILS_ARG_NN a_pMutex, QObject* CPPUTILS_ARG_NN a_pObj, const TypeFunc& a_func)
{
    if(a_pObj->thread() == QThread::currentThread()){
        a_func();
    }
    else{
        bool bShouldBeDeleted;
        BlockedCallerThreadId* pThreadDataCaller = (BlockedCallerThreadId*)CinternalTlsGetSpecific(a_pMutex->m_tls_data);
        if(pThreadDataCaller){
            bShouldBeDeleted = false;
        }
        else{
            // we are first caller, no blocked call invoked
            pThreadDataCaller = new BlockedCallerThreadId({nullptr,::std::this_thread::get_id(),false,{}});
            CinternalTlsSetSpecific(a_pMutex->m_tls_data,(void*)pThreadDataCaller);
            bShouldBeDeleted = true;
        }
        QMetaObject::invokeMethod(a_pObj,[a_pMutex,pThreadDataCaller,a_func](){
            bool bShouldBeDeletedIn;
            BlockedCallerThreadId* pThreadDataCallee = (BlockedCallerThreadId*)CinternalTlsGetSpecific(a_pMutex->m_tls_data);
            if(pThreadDataCallee){
                pThreadDataCallee->isBlockedCall = true;
                bShouldBeDeletedIn = false;
            }
            else{
                pThreadDataCallee = new BlockedCallerThreadId({pThreadDataCaller,::std::this_thread::get_id(),true,{}});
                CinternalTlsSetSpecific(a_pMutex->m_tls_data,(void*)pThreadDataCallee);
                bShouldBeDeletedIn = true;
            }
            a_func();
            if(bShouldBeDeletedIn){
                CinternalTlsSetSpecific(a_pMutex->m_tls_data,(void*)0);
                delete pThreadDataCallee;
            }
        },Qt::QueuedConnection);
        if(bShouldBeDeleted){
            CinternalTlsSetSpecific(a_pMutex->m_tls_data,(void*)0);
            delete pThreadDataCaller;
        }
    }  //  else of if(a_pObj->thread() == QThread::currentThread()){
}



}}}  //  namespace qtutils { namespace core{ namespace mutexandblockedinvoker{


#endif  //  #ifndef QTUTILS_INCLUDE_QTUTILS_CORE_IMPL_MUTEXANDBLOCKEDINVOKER_IMPL_HPP
