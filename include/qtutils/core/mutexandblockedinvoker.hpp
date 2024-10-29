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
#include <functional>
#include <stdint.h>
#include <qtutils/disable_utils_warnings.h>
#include <QObject>


namespace qtutils { namespace core{ namespace mutexandblockedinvoker{


typedef ::std::function<void(void)>  TypeFunc;
template <typename BaseMutex>
class Mutex_p;
template <typename BaseMutex>
class CInvoke_p;
template <typename BaseMutex>
class CInvoke;


template <typename BaseMutex>
class Mutex : public BaseMutex
{
    friend class CInvoke<BaseMutex>;
public:
    virtual ~Mutex();
    template<typename... Targs>
    Mutex(Targs... a_args);
    
    template<typename... Targs>
    void lock(Targs... a_args);
    template<typename... Targs>
    void unlock(Targs... a_args);
            
protected:
    Mutex_p<BaseMutex>* const   m_data_p;
};


template <typename BaseMutex>
class CInvoke 
{    
public:
    virtual ~CInvoke();
    CInvoke(Mutex<BaseMutex>* CPPUTILS_ARG_NN a_pCarier, QObject* CPPUTILS_ARG_NN a_pObj, const TypeFunc& a_func);
    
private:
    CInvoke(const CInvoke&) = delete;
    CInvoke(CInvoke&&) = delete;
    CInvoke& operator=(const CInvoke&) = delete;
    CInvoke& operator=(CInvoke&&) = delete;
    
private:
    CInvoke_p<BaseMutex>* const     m_data_p;
};


}}}  //  namespace qtutils { namespace core{ namespace lockandblockedinvoker{


#ifndef QTUTILS_INCLUDE_QTUTILS_CORE_IMPL_MUTEXANDBLOCKEDINVOKER_IMPL_HPP
#include <qtutils/core/impl/qtutils_core_mutexandblockedinvoker.impl.hpp>
#endif


#endif  //  #ifdef QTUTILS_INCLUDE_QTUTILS_CORE_MUTEXANDBLOCKEDINVOKER_HPP
