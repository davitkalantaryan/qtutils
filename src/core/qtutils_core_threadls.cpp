//
// file:            qtutils_threadls.cpp
// path:			src/core/focust_threadls.cpp
// created on:		2022 Jan 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#include <qtutils/core/threadls.hpp>

#ifndef QTUTILS_NOT_USE_THREADLS

#include <cpputils/inscopecleaner.hpp>
#include <cpputils/unnamedsemaphore.hpp>


namespace qtutils{

static void StaticConstruct(void*){}
static void StaticDestruct(void*){}


class CPPUTILS_DLL_PRIVATE ThreadLS_p final : public QThread
{
public:
    ThreadLS_p(const ThreadLS::TypeConstruct& a_construct, const ThreadLS::TypeDestruct& a_destruct, void* a_pData);
public:
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
    m_thr_data_p->start();
    m_thr_data_p->m_pSema->Wait();
    delete m_thr_data_p->m_pSema;
    m_thr_data_p->m_pSema = nullptr;
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
    });

    m_construct(m_userData);
    m_pSema->Post();

    QThread::exec();
}


}  // namespace qtutils{


#endif  // #ifndef QTUTILS_NOT_USE_THREADLS
