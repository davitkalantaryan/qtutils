//
// repo:            qtutils
// file:            clsinvokeblocked.hpp
// path:			include/qtutils/core/clsinvokeblocked.hpp
// created on:		2024 May 15
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once


#include <qtutils/export_symbols.h>
#include <functional>
#include <stdint.h>
#include <qtutils/disable_utils_warnings.h>
#include <QObject>


namespace qtutils { namespace core{ namespace blocking{


typedef ::std::function<void(void)>  TypeFunc;
class CPPUTILS_DLL_PRIVATE Carrier_p;
class CPPUTILS_DLL_PRIVATE CInvoke_p;
class QTUTILS_EXPORT CInvoke;


class QTUTILS_EXPORT Carrier final
{
    friend class CInvoke;
public:
    ~Carrier();
    Carrier();
    
    void lock();
    void unlock();
        
private:
    Carrier(const Carrier&) = delete;
    Carrier(Carrier&&) = delete;
    Carrier& operator=(const Carrier&) = delete;
    Carrier& operator=(Carrier&&) = delete;
    
private:
    Carrier_p* const  m_data_p;
};


class QTUTILS_EXPORT CInvoke final 
{    
public:
    ~CInvoke();
    CInvoke(Carrier* CPPUTILS_ARG_NN a_pCarier, QObject* CPPUTILS_ARG_NN a_pObj, const TypeFunc& a_func);
    
private:
    CInvoke(const CInvoke&) = delete;
    CInvoke(CInvoke&&) = delete;
    CInvoke& operator=(const CInvoke&) = delete;
    CInvoke& operator=(CInvoke&&) = delete;
    
private:
    CInvoke_p* const    m_data_p;
};


}}}  //  namespace qtutils { namespace core{ namespace blocking{
