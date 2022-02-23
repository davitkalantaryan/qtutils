//
// file:            initdeinit.hpp
// path:			include/qtutils/core/initdeinit.hpp
// created on:		2022 Feb 23
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once


#include <qtutils/qtutils_internal_header.h>

#ifndef QTUTILS_NOT_USE_INITDEINIT

#include <functional>

namespace qtutils{

class CPPUTILS_DLL_PRIVATE InitDeinit_p;

class QTUTILS_EXPORT InitDeinit final
{
public:
    //typedef void (*TypeConstruct)(void*);
    //typedef void (*TypeDestruct)(void*);
	typedef ::std::function<void(void*)>	TypeConstruct;
	typedef ::std::function<void(void*)>	TypeDestruct;
	
public:
    InitDeinit();
    InitDeinit(const TypeConstruct& a_construct, const TypeDestruct& a_destruct, void* a_data);
    ~InitDeinit();
    InitDeinit(const InitDeinit&)=delete;
    InitDeinit(InitDeinit&&);
    InitDeinit& operator=(const InitDeinit&)=delete;
    InitDeinit& operator=(InitDeinit&&);

private:
    InitDeinit_p*        m_id_data_p;
};


}  // namespace qtutils{

#endif  // #ifndef QTUTILS_NOT_USE_INITDEINIT
