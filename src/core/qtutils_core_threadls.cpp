//
// file:            qtutils_threadls.cpp
// path:			src/core/focust_threadls.cpp
// created on:		2022 Jan 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#include <qtutils/core/threadls.hpp>

#ifndef QTUTILS_NOT_USE_THREADLS

#include <cpputils/inscopecleaner.hpp>
#include <stddef.h>
#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#define SleepInterruptible(...)         SleepEx(INFINITE,TRUE)
#define qtutils_pthread_self            GetCurrentThread
#define QtutilsInterruptThread(_thr)    QueueUserAPC([](_In_ ULONG_PTR){},(_thr),0)
typedef HANDLE  qtutils_pthread_t;
#else
#include <signal.h>
#define SleepInterruptible(...)         sigsuspend(&aSigset)
#define qtutils_pthread_self            pthread_self
#define QtutilsInterruptThread(_thr)    pthread_kill((_thr),SIGUSR1)
typedef pthread_t   qtutils_pthread_t;
#endif

#include <qtutils/disable_utils_warnings.h>
#include <QDebug>


namespace qtutils{

static void StaticConstruct(void*){}
static void StaticDestruct(void*){}


class CPPUTILS_DLL_PRIVATE ThreadLS_p final : public QThread
{
public:
    ThreadLS_p(const ThreadLS::TypeConstruct& a_construct, const ThreadLS::TypeDestruct& a_destruct, void* a_pData);
public:
    union{
        size_t  all;
        struct{
            size_t shouldWait : 1;
            //size_t reserved01 : 63;
        }b;
    }m_flags;
private:
    const ThreadLS::TypeConstruct     m_construct;
    const ThreadLS::TypeDestruct      m_destruct;
    void*const                        m_userData;
    const qtutils_pthread_t           m_parentThread;
private:
    void run() override;
};


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

ThreadLS::ThreadLS()
    :
      m_thr_data_p(nullptr)
{
}


ThreadLS::ThreadLS(ThreadLS&& a_mM)
	:
	  m_thr_data_p(a_mM.m_thr_data_p)
{
	a_mM.m_thr_data_p = nullptr;
}


ThreadLS::ThreadLS(const TypeConstruct& a_construct, const TypeDestruct& a_destruct, void* a_pData)
    :
      m_thr_data_p(new ThreadLS_p(a_construct,a_destruct,a_pData))
{

#ifdef _WIN32
#else
    sigset_t aSigset;
    struct sigaction aNewAction, aOldAction;

    sigemptyset(&aNewAction.sa_mask);
    aNewAction.sa_flags = 0;
    aNewAction.sa_handler = [](int){};
    sigaction(SIGUSR1, &aNewAction, &aOldAction);

    sigfillset(&aSigset);
    sigdelset(&aSigset, SIGUSR1);

#endif

    m_thr_data_p->start();

    while(m_thr_data_p->m_flags.b.shouldWait){
        SleepInterruptible();
        qDebug()<<m_thr_data_p->m_flags.b.shouldWait;
    }
}


ThreadLS::~ThreadLS()
{
	if(m_thr_data_p){
		m_thr_data_p->quit();
		m_thr_data_p->wait();
		delete m_thr_data_p;
	}
}


ThreadLS& ThreadLS::operator=(ThreadLS&& a_mM)
{
	ThreadLS_p* this_thr_data_p = m_thr_data_p;
	m_thr_data_p = a_mM.m_thr_data_p;
	a_mM.m_thr_data_p = this_thr_data_p;
	return *this;
}


QThread* ThreadLS::qThread()const
{
    return m_thr_data_p;
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/


ThreadLS_p::ThreadLS_p(const ThreadLS::TypeConstruct& a_construct, const ThreadLS::TypeDestruct& a_destruct, void* a_pData)
    :
      m_construct(a_construct?a_construct:(&StaticConstruct)),
      m_destruct(a_destruct?a_destruct:(&StaticDestruct)),
      m_userData(a_pData),
      m_parentThread(qtutils_pthread_self())
{
    qDebug()<<qtutils_pthread_self();
    m_flags.all = 0;
    m_flags.b.shouldWait = 1;
}


void ThreadLS_p::run()
{   
    cpputils::InScopeCleaner aCleaner([this](void*){
        m_destruct(m_userData);
    });

    m_construct(m_userData);


    //m_pSema->Post();
    m_flags.b.shouldWait = 0;
    //QtutilsInterruptThread(m_parentThread);
    qDebug()<<m_parentThread;
    QueueUserAPC([](_In_ ULONG_PTR){},m_parentThread,0);

    QThread::exec();
}


}  // namespace qtutils{


#endif  // #ifndef QTUTILS_NOT_USE_THREADLS
