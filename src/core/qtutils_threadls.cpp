//
// file:            focust_threadls.cpp
// path:			src/core/focust_threadls.cpp
// created on:		2022 Jan 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#include <focust/core/threadls.hpp>
#include <cpputils/inscopecleaner.hpp>
#include <cpputils/unnamedsemaphore.hpp>


namespace focust{

static void StaticConstruct(void*){}
static void StaticDestruct(void*){}


class FOCUST_DLL_PRIVATE ThreadLS_p : public QThread
{
public:
    ThreadLS_p(const ThreadLS::TypeConstruct& a_construct, const ThreadLS::TypeDestruct& a_destruct, void* a_pData);
public:
    ThreadObject*               m_pSockThreadObj;
    cpputils::UnnamedSemaphore* m_pSema;
private:
    const ThreadLS::TypeConstruct     m_construct;
    const ThreadLS::TypeDestruct      m_destruct;
    void*const                        m_userData;
private:
    void run() override;
};


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

ThreadLS::ThreadLS()
    :
      ThreadLS(nullptr,nullptr,nullptr)
{
}


ThreadLS::ThreadLS(const TypeConstruct& a_construct, const TypeDestruct& a_destruct, void* a_pData)
    :
      m_thr_data_p(new ThreadLS_p(a_construct,a_destruct,a_pData))
{
    m_thr_data_p->start();
    m_thr_data_p->m_pSema->Wait();
    delete m_thr_data_p->m_pSema;
    m_thr_data_p->m_pSema = nullptr;
}


ThreadLS::~ThreadLS()
{
    m_thr_data_p->quit();
    m_thr_data_p->wait();
    delete m_thr_data_p;
}


ThreadObject* ThreadLS::thrObj()const
{
    return m_thr_data_p->m_pSockThreadObj;
}


QThread* ThreadLS::qThread()const
{
    return m_thr_data_p;
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/


ThreadLS_p::ThreadLS_p(const ThreadLS::TypeConstruct& a_construct, const ThreadLS::TypeDestruct& a_destruct, void* a_pData)
    :
      m_pSockThreadObj(nullptr),
      m_pSema(new ::cpputils::UnnamedSemaphore()),
      m_construct(a_construct?a_construct:(&StaticConstruct)),
      m_destruct(a_destruct?a_destruct:(&StaticDestruct)),
      m_userData(a_pData)
{
}


void ThreadLS_p::run()
{   
    cpputils::InScopeCleaner aCleaner([this](void*){
        m_destruct(m_userData);
        delete m_pSockThreadObj;
        m_pSockThreadObj = nullptr;
    });

    m_pSockThreadObj = new ThreadObject();
    m_construct(m_userData);
    m_pSema->Post();

    QThread::exec();
}


}  // namespace focust{
