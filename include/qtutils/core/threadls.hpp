//
// file:            websockscontainer.hpp
// path:			include/focust/core/websockscontainer.hpp
// created on:		2022 Jan 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#include <qtutils/export_symbols.h>

#ifndef QTUTILS_NOT_USE_THREADLS

#include <cinternal/disable_compiler_warnings.h>
#include <functional>
#include <thread>
#include <qtutils/disable_utils_warnings.h>
#include <QThread>
#include <cinternal/undisable_compiler_warnings.h>

// LS stands for Late Start, constructor exits when thread stated

namespace qtutils{ namespace core{

class CPPUTILS_DLL_PRIVATE ThreadLS_p;

class QTUTILS_EXPORT ThreadLS final
{
public:
	typedef ::std::function<void(void*)>	TypeConstruct;
	typedef ::std::function<void(void*)>	TypeDestruct;
    typedef ::std::function<void(void*)>	TypeMain;
	
public:
    ~ThreadLS();
    ThreadLS();
    ThreadLS(const TypeConstruct& a_construct, const TypeDestruct& a_destruct, void* a_data);
    ThreadLS(const TypeMain& a_main, const TypeConstruct& a_construct, const TypeDestruct& a_destruct, void* a_data);
    ThreadLS(ThreadLS&&) noexcept;
    ThreadLS& operator=(ThreadLS&&) noexcept;

    QThread* qThread() noexcept;
    const QThread* qThread()const noexcept;
    void EnableExceptionsHandling() noexcept;
    void DisableExceptionsHandling() noexcept;
    bool hasExceptionHandling()const noexcept;
    ::std::thread::native_handle_type getNativeHandle()const noexcept;
    ::std::thread::native_handle_type GetAndResetNativeHandle() noexcept;

private:
    ThreadLS_p*        m_thr_data_p;

private:
    ThreadLS(const ThreadLS&)=delete;
    ThreadLS& operator=(const ThreadLS&)=delete;
};


}  //  namespace core


typedef core::ThreadLS ThreadLS;


}  //  namespace qtutils{

#endif  // #ifndef QTUTILS_NOT_USE_THREADLS
