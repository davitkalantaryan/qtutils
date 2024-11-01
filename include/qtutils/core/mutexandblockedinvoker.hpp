//
// repo:            qtutils
// file:            mutexandblockedinvoker.hpp
// path:			include/qtutils/core/mutexandblockedinvoker.hpp
// created on:		2024 Oct 29
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once


#ifndef QTUTILS_INCLUDE_QTUTILS_CORE_MUTEXANDBLOCKEDINVOKER_HPP
#define QTUTILS_INCLUDE_QTUTILS_CORE_MUTEXANDBLOCKEDINVOKER_HPP

#include <qtutils/export_symbols.h>
#include <cinternal/thread_local_sys.h>
#include <functional>
#include <thread>
#include <stdint.h>
#include <qtutils/disable_utils_warnings.h>
#include <QObject>


namespace qtutils { namespace core{ namespace mutexandblockedinvoker{


typedef ::std::function<void(void)>  TypeFunc;
// BaseMutex has following functions
// 1. [any_type] lock (Targs... a_args)
// 2. [any_type] unlock (Targs... a_args)
// 3. [any type] set_locker_thread_id(const ::std::thread::id& a_id, Targs... a_args) 
// 5. ::std::thread::id get_locker_thread_id(Targs... a_args)const    -> should be reentrant (?)
// 6. [any type] increment_lock_count(Targs... a_args)    
// 7. ptrdiff_t decrement_lock_count_and_return(Targs... a_args)    
template <typename BaseMutex, typename... Targs>
class Mutex;

typedef size_t IntType;

template <typename BaseMutex,typename... Targs>
static inline void InvokeBlocked(Mutex<BaseMutex,Targs...>* CPPUTILS_ARG_NN a_pMutex, QObject* CPPUTILS_ARG_NN a_pObj, const TypeFunc& a_func);

template <typename BaseMutex>
class SimpleBaseMutex;
template <typename BaseMutex>
using SimpleMutex = Mutex< SimpleBaseMutex<BaseMutex> >;

template <typename BaseMutex>
class ArrayBaseMutex;
template <typename BaseMutex, typename IntType = size_t>
using ArrayMutex = Mutex< ArrayBaseMutex<BaseMutex>, IntType >;


/*/////////////////////////////////////////////////////////////////////////////////*/

template <typename BaseMutex,typename... Targs>
class Mutex : public BaseMutex
{
    friend void InvokeBlocked<BaseMutex,Targs...>(Mutex<BaseMutex,Targs...>* CPPUTILS_ARG_NN a_pMutex, QObject* CPPUTILS_ARG_NN a_pObj, const TypeFunc& a_func);
public:
    virtual ~Mutex();
    template <typename... TargsMtx>
    Mutex(TargsMtx... a_mtxArgs);
    Mutex(const Mutex&)=delete;
    Mutex& operator=(const Mutex&)=delete;
    
    void lock(Targs... a_args);
    void unlock(Targs... a_args);
            
protected:
    BaseMutex                   m_mutex;
    CinternalTlsData            m_tls_data;
};


template <typename BaseMutex>
class SimpleBaseMutex : public BaseMutex
{
public:
    template <typename... TargsMtx>
    SimpleBaseMutex(TargsMtx... a_mtxArgs);
    SimpleBaseMutex(const SimpleBaseMutex&)=delete;
    SimpleBaseMutex& operator=(const SimpleBaseMutex&)=delete;
    
    void lock();
    void unlock();
    void set_locker_thread_id(const ::std::thread::id& a_id);
    ::std::thread::id get_locker_thread_id()const;
    void increment_lock_count();
    ptrdiff_t decrement_lock_count_and_return();
            
private:
    ::std::thread::id   m_threadId;
    ptrdiff_t           m_lockCount;;
};



template <typename BaseMutex>
class ArrayBaseMutex : public BaseMutex
{
public:
    ~ArrayBaseMutex();
    template <typename... TargsMtx>
    ArrayBaseMutex(TargsMtx... a_mtxArgs);
    ArrayBaseMutex(const ArrayBaseMutex&)=delete;
    ArrayBaseMutex& operator=(const ArrayBaseMutex&)=delete;
    
    void lock(const IntType& a_index);
    void unlock(const IntType& a_index);
    void set_locker_thread_id(const ::std::thread::id& a_id, const IntType& a_index);
    ::std::thread::id get_locker_thread_id(const IntType& a_index)const;
    void increment_lock_count(const IntType& a_index);
    ptrdiff_t decrement_lock_count_and_return(const IntType& a_index);

private:
    struct SItem{
        ::std::thread::id   threadId;
        ptrdiff_t           lockCount;
    };
private:
    SItem*      m_pLockData;
};



}}}  //  namespace qtutils { namespace core{ namespace lockandblockedinvoker{


#ifndef QTUTILS_INCLUDE_QTUTILS_CORE_IMPL_MUTEXANDBLOCKEDINVOKER_IMPL_HPP
#include <qtutils/core/impl/qtutils_core_mutexandblockedinvoker.impl.hpp>
#endif


#endif  //  #ifdef QTUTILS_INCLUDE_QTUTILS_CORE_MUTEXANDBLOCKEDINVOKER_HPP
