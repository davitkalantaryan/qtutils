//
// file:            websockscontainer.hpp
// path:			include/focust/core/websockscontainer.hpp
// created on:		2022 Jan 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once


#include <focust/core/focust_internal_header.h>
#include <focust/core/threadobject.hpp>
#include <focust/core/disable_utils_warnings.h>
#include <QThread>

// LS stands for Late Start, constructor exits when thread stated

namespace focust{

class FOCUST_DLL_PRIVATE ThreadLS_p;

class FOCUST_EXPORT ThreadLS final
{
public:
    typedef void (*TypeConstruct)(void*);
    typedef void (*TypeDestruct)(void*);
public:
    ThreadLS();
    ThreadLS(const TypeConstruct& a_construct, const TypeDestruct& a_destruct, void* a_data);
    ~ThreadLS();
    ThreadLS(const ThreadLS&)=delete;
    ThreadLS(ThreadLS&&)=delete;
    ThreadLS& operator=(const ThreadLS&)=delete;
    ThreadLS& operator=(ThreadLS&&)=delete;

    ThreadObject* thrObj()const;
    QThread* qThread()const;

private:
    ThreadLS_p*const        m_thr_data_p;
};

}  // namespace focust{
