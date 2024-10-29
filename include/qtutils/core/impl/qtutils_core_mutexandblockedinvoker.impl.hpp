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
#include <qtutils/disable_utils_warnings.h>
#include <QMetaObject>
#include <QThread>

namespace qtutils { namespace core{ namespace mutexandblockedinvoker{


static void PthreadKeyCreateDeleteClbk(void*){}


template <typename BaseMutex>
class Mutex_p{
public:
    CinternalTlsData                m_tls_data;
    
public:
    inline void IncrementCurrentValue(size_t a_currentValue);
    inline size_t DecrementCurrentValue();
};


template <typename BaseMutex>
class CInvoke_p{
public:
    CInvoke_p(Mutex<BaseMutex>* CPPUTILS_ARG_NN a_mutex_p);

public:
    Mutex<BaseMutex>* const   mutex_p;
};


/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////*/


template <typename BaseMutex>
CInvoke_p<BaseMutex>::CInvoke_p(Mutex<BaseMutex>* CPPUTILS_ARG_NN a_mutex_p)
    :
      mutex_p(a_mutex_p)
{
}


template <typename BaseMutex>
inline void Mutex_p<BaseMutex>::IncrementCurrentValue(size_t a_currentValue)
{
    ++a_currentValue;
    CinternalTlsSetSpecific(this->m_tls_data,(void*)a_currentValue);
}


template <typename BaseMutex>
inline size_t Mutex_p<BaseMutex>::DecrementCurrentValue()
{
    size_t currentValue = (size_t)CinternalTlsGetSpecific(this->m_tls_data);
    assert(currentValue);
    --currentValue;
    CinternalTlsSetSpecific(this->m_tls_data,(void*)currentValue);
    return currentValue;
}


/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

template <typename BaseMutex>
Mutex<BaseMutex>::~Mutex()
{
    CinternalTlsDelete(m_data_p->m_tls_data);
    delete m_data_p;
}


template <typename BaseMutex>
template<typename... Targs>
Mutex<BaseMutex>::Mutex(Targs... a_args)
    :
      BaseMutex(a_args...),
      m_data_p(new Mutex_p<BaseMutex>())
{
    CinternalTlsAlloc(&(m_data_p->m_tls_data),&PthreadKeyCreateDeleteClbk);
    if(m_data_p->m_tls_data==CINTERNAL_TLS_OUT_OF_INDEXES){
        throw "Failure on constructor. Unable to create TLS";
    }
}


template <typename BaseMutex>
template<typename... Targs>
void Mutex<BaseMutex>::lock(Targs... a_args)
{
    size_t currentValue = (size_t)CinternalTlsGetSpecific(m_data_p->m_tls_data);
    if(currentValue){
        m_data_p->IncrementCurrentValue(currentValue);
    }
    else{
        BaseMutex::lock(a_args...);
        CinternalTlsSetSpecific(m_data_p->m_tls_data,(void*)1);
    }
}


template <typename BaseMutex>
template<typename... Targs>
void Mutex<BaseMutex>::unlock(Targs... a_args)
{
    if(!(m_data_p->DecrementCurrentValue())){
        BaseMutex::unlock(a_args...);
    }
}


/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

template <typename BaseMutex>
CInvoke<BaseMutex>::~CInvoke()
{
    delete m_data_p;
}


template <typename BaseMutex>
CInvoke<BaseMutex>::CInvoke(Mutex<BaseMutex>* CPPUTILS_ARG_NN a_pMutex, QObject* CPPUTILS_ARG_NN a_pObj, const TypeFunc& a_func)
    :
      m_data_p(new CInvoke_p<BaseMutex>(a_pMutex))
{
    size_t currentValue = (size_t)CinternalTlsGetSpecific(m_data_p->mutex_p->m_data_p->m_tls_data);
    if(a_pObj->thread() == QThread::currentThread()){
        m_data_p->mutex_p->m_data_p->IncrementCurrentValue(currentValue);
        a_func();
        m_data_p->mutex_p->m_data_p->DecrementCurrentValue();
    }
    else{
        if(currentValue){
            Mutex_p<BaseMutex>* const mutex_p = m_data_p->mutex_p->m_data_p;
            QMetaObject::invokeMethod(a_pObj,[mutex_p,a_func](){
                size_t currentValueIn = (size_t)CinternalTlsGetSpecific(mutex_p->m_tls_data);
                mutex_p->IncrementCurrentValue(currentValueIn);
                a_func();
                mutex_p->DecrementCurrentValue();
            },Qt::QueuedConnection);
        }
        else{
            m_data_p->mutex_p->m_data_p->IncrementCurrentValue(currentValue);
            QMetaObject::invokeMethod(a_pObj,[this,a_func](){
                size_t currentValueIn = (size_t)CinternalTlsGetSpecific(m_data_p->mutex_p->m_data_p->m_tls_data);
                m_data_p->mutex_p->m_data_p->IncrementCurrentValue(currentValueIn);
                a_func();
                m_data_p->mutex_p->m_data_p->DecrementCurrentValue();
            },Qt::BlockingQueuedConnection);
            m_data_p->mutex_p->m_data_p->DecrementCurrentValue();
        }
    }    
}



}}}  //  namespace qtutils { namespace core{ namespace mutexandblockedinvoker{


#endif  //  #ifndef QTUTILS_INCLUDE_QTUTILS_CORE_IMPL_MUTEXANDBLOCKEDINVOKER_IMPL_HPP
