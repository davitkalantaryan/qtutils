//
// file:            qtutils_threadls.cpp
// path:			src/core/focust_threadls.cpp
// created on:		2022 Jan 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#include <qtutils/core/threadls.hpp>

#ifndef QTUTILS_NOT_USE_THREADLS

#define cinternal_unnamed_sema_wait_ms_needed
#include <cinternal/unnamed_semaphore.h>
#include <cinternal/bistateflags.h>
#include <cinternal/disable_compiler_warnings.h>
#include <memory>
#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#else
#include <signal.h>
#include <pthread.h>
#endif
#include <qtutils/disable_utils_warnings.h>
#include <QEventLoop>
#include <QTimer>
#include <QEvent>
#include <QCoreApplication>
#include <cinternal/undisable_compiler_warnings.h>


namespace qtutils{ namespace core{

static void StaticConstruct(void*){}
static void StaticDestruct(void*){}


class CPPUTILS_DLL_PRIVATE ThreadStopEvent final : public QEvent
{
public:
    ThreadStopEvent();
private:
    ThreadStopEvent(const ThreadStopEvent&)=delete;
    ThreadStopEvent(ThreadStopEvent&&)=delete;
    ThreadStopEvent& operator=(const ThreadStopEvent&)=delete;
    ThreadStopEvent& operator=(ThreadStopEvent&&)=delete;
};


class CPPUTILS_DLL_PRIVATE WaitEventLoop final : public QEventLoop
{
public:
    WaitEventLoop(ThreadLS_p* CPPUTILS_ARG_NN a_threadLs_p);

private:
    bool event(QEvent* a_event) override;

private:
    ThreadLS_p* const   m_threadLs_p;
};


class CPPUTILS_DLL_PRIVATE ThreadLS_p final : public QThread
{
public:
    ThreadLS_p(const ThreadLS::TypeConstruct& a_construct, const ThreadLS::TypeDestruct& a_destruct, void* a_pData, const ThreadLS::TypeMain& a_main);
    ThreadLS_p(const ThreadLS::TypeConstruct& a_construct, const ThreadLS::TypeDestruct& a_destruct, void* a_pData);
public:
    cinternal_unnamed_sema_t            m_sema;
    ::std::thread::native_handle_type   m_nativeHandle;
    WaitEventLoop                       m_finalLoop;
    CPPUTILS_BISTATE_FLAGS_UN(
        shouldRun,
        hasExceptionHandling,
        loopFinished
    )flags;
private:
    const ThreadLS::TypeConstruct       m_construct;
    const ThreadLS::TypeDestruct        m_destruct;
    const ThreadLS::TypeMain            m_main;
    void*const                          m_userData;
private:
    void run() override;
    void DefaultMain(void*);
private:
    ThreadLS_p(const ThreadLS_p&);
    ThreadLS_p(ThreadLS_p&&);
    ThreadLS_p& operator=(const ThreadLS_p&);
    ThreadLS_p& operator=(ThreadLS_p&&);
};


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/


ThreadLS::~ThreadLS()
{
    if(m_thr_data_p){
        ThreadLS_p* const thr_data_p = m_thr_data_p;
        m_thr_data_p = nullptr;
        if(thr_data_p->flags.rd.shouldRun_true){
            thr_data_p->flags.wr.shouldRun = CPPUTILS_BISTATE_MAKE_BITS_FALSE;
            thr_data_p->quit();
        }  //  if(thr_data_p->flags.rd.shouldRun_true){
        thr_data_p->wait();
        delete thr_data_p;
    }  //  if(m_thr_data_p){
}


ThreadLS::ThreadLS()
    :
      m_thr_data_p(nullptr)
{
}


ThreadLS::ThreadLS(ThreadLS&& a_mM) noexcept
	:
	  m_thr_data_p(a_mM.m_thr_data_p)
{
	a_mM.m_thr_data_p = nullptr;
}


ThreadLS::ThreadLS(const TypeConstruct& a_construct, const TypeDestruct& a_destruct, void* a_pData)
    :
      m_thr_data_p(new ThreadLS_p(a_construct,a_destruct,a_pData))
{
    m_thr_data_p->start();
    cinternal_unnamed_sema_wait(&(m_thr_data_p->m_sema));
    cinternal_unnamed_sema_destroy(&(m_thr_data_p->m_sema));
}


ThreadLS::ThreadLS(const TypeMain& a_main, const TypeConstruct& a_construct, const TypeDestruct& a_destruct, void* a_pData)
    :
      m_thr_data_p(new ThreadLS_p(a_construct,a_destruct,a_pData,a_main))
{
    m_thr_data_p->start();
    cinternal_unnamed_sema_wait(&(m_thr_data_p->m_sema));
    cinternal_unnamed_sema_destroy(&(m_thr_data_p->m_sema));
}


ThreadLS& ThreadLS::operator=(ThreadLS&& a_mM) noexcept
{
	ThreadLS_p* this_thr_data_p = m_thr_data_p;
	m_thr_data_p = a_mM.m_thr_data_p;
	a_mM.m_thr_data_p = this_thr_data_p;
	return *this;
}


QThread* ThreadLS::qThread() noexcept
{
    return m_thr_data_p;
}


const QThread* ThreadLS::qThread()const noexcept
{
    return m_thr_data_p;
}


void ThreadLS::EnableExceptionsHandling() noexcept
{
    m_thr_data_p->flags.wr.hasExceptionHandling = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
}


void ThreadLS::DisableExceptionsHandling() noexcept
{
    m_thr_data_p->flags.wr.hasExceptionHandling = CPPUTILS_BISTATE_MAKE_BITS_FALSE;
}


bool ThreadLS::hasExceptionHandling()const noexcept
{
    return static_cast<bool>(m_thr_data_p->flags.rd.hasExceptionHandling_true);
}


bool ThreadLS::loopFinished() const noexcept
{
    return static_cast<bool>(m_thr_data_p->flags.rd.loopFinished_true);
}


bool ThreadLS::loopNotFinished() const noexcept
{
    return static_cast<bool>(m_thr_data_p->flags.rd.loopFinished_false);
}


::std::thread::native_handle_type ThreadLS::getNativeHandle()const noexcept
{
    return m_thr_data_p->m_nativeHandle;
}


::std::thread::native_handle_type ThreadLS::GetAndResetNativeHandle() noexcept
{
    const ::std::thread::native_handle_type nativeHandle = m_thr_data_p->m_nativeHandle;
    m_thr_data_p->m_nativeHandle = (::std::thread::native_handle_type)0;
    return nativeHandle;
}


bool ThreadLS::StopThreadWithSignalAndProperWait(void* a_sigNo, int a_timeoutMs)
{
    if(m_thr_data_p){
        const ::std::thread::native_handle_type threadNativeHandle = m_thr_data_p->m_nativeHandle;
        if(m_thr_data_p->flags.rd.shouldRun_true){
            m_thr_data_p->flags.wr.shouldRun = CPPUTILS_BISTATE_MAKE_BITS_FALSE;
            m_thr_data_p->quit();
        }  //  if(m_thr_data_p->flags.rd.shouldRun_true){

#ifdef _WIN32
        if(threadNativeHandle){
            QueueUserAPC([](_In_ ULONG_PTR) {},(HANDLE)threadNativeHandle,((ULONG_PTR)((size_t)a_sigNo)));
        }
#else
        if(threadNativeHandle){
            pthread_kill((pthread_t)threadNativeHandle,((int)((size_t)a_sigNo)));
        }
#endif
        if(m_thr_data_p->flags.rd.loopFinished_false){
            if(a_timeoutMs>=0){
                QTimer tmrEL;
                QObject::connect(&tmrEL, &QTimer::timeout, &m_thr_data_p->m_finalLoop,[this](){
                    m_thr_data_p->m_finalLoop.quit();
                });  //  QObject::connect(&tmrEL, &QTimer::timeout, &loop,[this,&loop](){
                tmrEL.start(a_timeoutMs);
            }  //  if(a_timeoutMs>=0){
            m_thr_data_p->m_finalLoop.exec();
        }  //  if(m_thr_data_p->flags.rd.loopFinished_false){
        return static_cast<bool>(m_thr_data_p->flags.rd.loopFinished_true);
    }  //  if(m_thr_data_p){

    return true;
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/


ThreadLS_p::ThreadLS_p(const ThreadLS::TypeConstruct& a_construct, const ThreadLS::TypeDestruct& a_destruct, void* a_pData, const ThreadLS::TypeMain& a_main)
    :
    m_finalLoop(this),
    m_construct(a_construct?a_construct:(&StaticConstruct)),
    m_destruct(a_destruct?a_destruct:(&StaticDestruct)),
    m_main(a_main?a_main:([this](void* a_pData){DefaultMain(a_pData);})),
    m_userData(a_pData)
{
    this->flags.wr_all = CPPUTILS_BISTATE_MAKE_ALL_BITS_FALSE;
    this->flags.wr.shouldRun = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
    this->flags.wr.hasExceptionHandling = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
    m_nativeHandle = (::std::thread::native_handle_type)0;
    cinternal_unnamed_sema_create(&(this->m_sema),0);
}


ThreadLS_p::ThreadLS_p(const ThreadLS::TypeConstruct& a_construct, const ThreadLS::TypeDestruct& a_destruct, void* a_pData)
    :
    ThreadLS_p(a_construct,a_destruct,a_pData,[this](void* a_pData){DefaultMain(a_pData);})
{
}


void ThreadLS_p::run()
{   
    //cpputils::InScopeCleaner aCleaner([this](void*){
    //    m_destruct(m_userData);
    //});

    ::std::unique_ptr<ThreadLS_p, void(*)(ThreadLS_p*)> aCleaner(this, [](ThreadLS_p* a_this){
		a_this->m_destruct(a_this->m_userData);
        a_this->m_nativeHandle = (::std::thread::native_handle_type)0;
	});

#ifdef _WIN32
    m_nativeHandle = (::std::thread::native_handle_type)GetCurrentThread();
#else
    m_nativeHandle = (::std::thread::native_handle_type)pthread_self();
#endif
	
    m_construct(m_userData);
    cinternal_unnamed_sema_post(&(this->m_sema));

    m_main(m_userData);
    flags.wr.loopFinished = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
    ThreadStopEvent* const pStopEvnt = new ThreadStopEvent();
    QCoreApplication::postEvent(&m_finalLoop,pStopEvnt);
}


void ThreadLS_p::DefaultMain(void*)
{
    do{
        try{
            QThread::exec();
        }
        catch(...){
            if(this->flags.rd.hasExceptionHandling_false){
                throw;
            }
        }
    }
    while((this->flags.rd.shouldRun_true) && (this->flags.rd.hasExceptionHandling_true));
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

static inline QEvent::Type GenerateAndGetEventTypeInline(){
    static const QEvent::Type evType = static_cast<QEvent::Type>(QEvent::registerEventType());
    return evType;
}


ThreadStopEvent::ThreadStopEvent()
    :
    QEvent(GenerateAndGetEventTypeInline())
{
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

WaitEventLoop::WaitEventLoop(ThreadLS_p* CPPUTILS_ARG_NN a_threadLs_p)
:
    m_threadLs_p(a_threadLs_p)
{
}


bool WaitEventLoop::event(QEvent* a_event)
{
    if(a_event->type()==GenerateAndGetEventTypeInline()){
        if(m_threadLs_p->flags.rd.loopFinished_true){
            this->quit();
        }
        return true;
    }  //  f(a_event->type()==GenerateAndGetEventTypeInline()){
    return QEventLoop::event(a_event);
}


}}  //  namespace qtutils{ namespace core{


#endif  // #ifndef QTUTILS_NOT_USE_THREADLS
