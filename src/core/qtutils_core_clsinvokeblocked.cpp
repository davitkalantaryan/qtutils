//
// repo:            qtutils
// file:            qtutils_core_clsinvokeblocked.cpp
// path:			src/core/qtutils_core_clsinvokeblocked.cpp
// created on:		2024 May 15
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/core/clsinvokeblocked.hpp>
#ifndef cinternal_lw_recursive_mutex_create_needed
#define cinternal_lw_recursive_mutex_create_needed
#endif
#include <cinternal/lw_mutex_recursive.h>
#include <cinternal/thread_local_sys.h>
#include <qtutils/disable_utils_warnings.h>
#include <QMetaObject>
#include <QThread>

namespace qtutils { namespace core{ namespace blocking{


static void PthreadKeyCreateDeleteClbk(void*){}


class CPPUTILS_DLL_PRIVATE Carrier_p{
public:
    cinternal_lw_recursive_mutex_t  m_mutex_rc;
    CinternalTlsData                m_tls_data2;
    //cinternal_lw_recursive_mutex_t  m_mutex_for_list_rc;
    
public:
    inline void IncrementCurrentValue(size_t a_currentValue);
    inline size_t DecrementCurrentValue();
    inline void LockInline();
    inline void UnlockInline();
};


class CPPUTILS_DLL_PRIVATE CInvoke_p{
public:
    Carrier_p*  carrier_p;
};


///


inline void Carrier_p::IncrementCurrentValue(size_t a_currentValue)
{
    ++a_currentValue;
    CinternalTlsSetSpecific(this->m_tls_data2,(void*)a_currentValue);
}

inline size_t Carrier_p::DecrementCurrentValue()
{
    size_t currentValue = (size_t)CinternalTlsGetSpecific(this->m_tls_data2);
    assert(currentValue);
    --currentValue;
    CinternalTlsSetSpecific(this->m_tls_data2,(void*)currentValue);
    return currentValue;
}


inline void Carrier_p::LockInline()
{
    size_t currentValue = (size_t)CinternalTlsGetSpecific(m_tls_data2);
    if(currentValue){
        IncrementCurrentValue(currentValue);
    }
    else{
        cinternal_lw_recursive_mutex_lock(&m_mutex_rc);
        CinternalTlsSetSpecific(m_tls_data2,(void*)1);
    }
}


inline void Carrier_p::UnlockInline()
{
    if(!DecrementCurrentValue()){
        cinternal_lw_recursive_mutex_unlock(&m_mutex_rc);
    }
}


///

Carrier::~Carrier()
{
    cinternal_lw_recursive_mutex_destroy(&(m_data_p->m_mutex_rc));
    CinternalTlsDelete(m_data_p->m_tls_data2);
    delete m_data_p;
}


Carrier::Carrier()
    :
      m_data_p(new Carrier_p())
{
    CinternalTlsAlloc(&(m_data_p->m_tls_data2),&PthreadKeyCreateDeleteClbk);
    if(m_data_p->m_tls_data2==CINTERNAL_TLS_OUT_OF_INDEXES){
        throw "Failure on constructor. Unable to create TLS";
    }
    if(cinternal_lw_recursive_mutex_create(&(m_data_p->m_mutex_rc))){
        throw "Failure on constructor. Unable to create mutex";
    }
}


void Carrier::lock()
{
    m_data_p->LockInline();
}


void Carrier::unlock()
{
    m_data_p->UnlockInline();
}


/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

CInvoke::~CInvoke()
{
    delete m_data_p;
}


CInvoke::CInvoke(Carrier* CPPUTILS_ARG_NN a_pCarier, QObject* CPPUTILS_ARG_NN a_pObj, const TypeFunc& a_func)
    :
      m_data_p(new CInvoke_p())
{
    m_data_p->carrier_p = a_pCarier->m_data_p;
    size_t currentValue = (size_t)CinternalTlsGetSpecific(m_data_p->carrier_p->m_tls_data2);
    if(a_pObj->thread() == QThread::currentThread()){
        m_data_p->carrier_p->IncrementCurrentValue(currentValue);
        a_func();
        m_data_p->carrier_p->DecrementCurrentValue();
    }
    else{
        if(currentValue){
            Carrier_p*  carrier_p = m_data_p->carrier_p;
            QMetaObject::invokeMethod(a_pObj,[carrier_p,a_func](){
                size_t currentValueIn = (size_t)CinternalTlsGetSpecific(carrier_p->m_tls_data2);
                carrier_p->IncrementCurrentValue(currentValueIn);
                a_func();
                carrier_p->DecrementCurrentValue();
            },Qt::QueuedConnection);
        }
        else{
            m_data_p->carrier_p->IncrementCurrentValue(currentValue);
            QMetaObject::invokeMethod(a_pObj,[this,a_func](){
                size_t currentValueIn = (size_t)CinternalTlsGetSpecific(m_data_p->carrier_p->m_tls_data2);
                m_data_p->carrier_p->IncrementCurrentValue(currentValueIn);
                a_func();
                m_data_p->carrier_p->DecrementCurrentValue();
            },Qt::BlockingQueuedConnection);
            m_data_p->carrier_p->DecrementCurrentValue();
        }
    }    
}



}}}  //  namespace qtutils { namespace core{ namespace blocking{
