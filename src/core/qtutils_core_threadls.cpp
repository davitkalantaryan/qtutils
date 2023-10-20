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
#include <memory>


namespace qtutils{

static void StaticConstruct(void*){}
static void StaticDestruct(void*){}


class CPPUTILS_DLL_PRIVATE ThreadLS_p final : public QThread
{
public:
    ThreadLS_p(const ThreadLS::TypeConstruct& a_construct, const ThreadLS::TypeDestruct& a_destruct, void* a_pData);
public:
    cinternal_unnamed_sema_t	m_sema;
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
    cinternal_unnamed_sema_wait(&(m_thr_data_p->m_sema));
    cinternal_unnamed_sema_destroy(&(m_thr_data_p->m_sema));
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
      m_userData(a_pData)
{
	cinternal_unnamed_sema_create(&(this->m_sema),0);
}


void ThreadLS_p::run()
{   
    //cpputils::InScopeCleaner aCleaner([this](void*){
    //    m_destruct(m_userData);
    //});
	
	std::unique_ptr<ThreadLS_p, void(*)(ThreadLS_p*)> aCleaner(this, [](ThreadLS_p* a_this){
		a_this->m_destruct(a_this->m_userData);
	});
	
    m_construct(m_userData);
    cinternal_unnamed_sema_post(&(this->m_sema));

    QThread::exec();
}


}  // namespace qtutils{


#endif  // #ifndef QTUTILS_NOT_USE_THREADLS
