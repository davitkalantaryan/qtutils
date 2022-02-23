//
// file:            qtutils_core_initdeinit.cpp
// path:			src/core/qtutils_core_initdeinit.cpp
// created on:		2022 Feb 23
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#include <qtutils/core/initdeinit.hpp>

#ifndef QTUTILS_NOT_USE_INITDEINIT


namespace qtutils{

static void StaticDestruct(void*){}


class CPPUTILS_DLL_PRIVATE InitDeinit_p final
{
public:
    InitDeinit_p(const InitDeinit::TypeDestruct& a_destruct, void* a_pData);
public:
    const InitDeinit::TypeDestruct    m_destruct;
    void*const                        m_userData;
};


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

InitDeinit::InitDeinit()
    :
      m_id_data_p(nullptr)
{
}


InitDeinit::InitDeinit(InitDeinit&& a_mM)
	:
	  m_id_data_p(a_mM.m_id_data_p)
{
	a_mM.m_id_data_p = nullptr;
}


InitDeinit::InitDeinit(const TypeConstruct& a_construct, const TypeDestruct& a_destruct, void* a_pData)
    :
      m_id_data_p(new InitDeinit_p(a_destruct,a_pData))
{
	if(a_construct){
		a_construct(a_pData);
	}	
}


InitDeinit::~InitDeinit()
{
	if(m_id_data_p){
		m_id_data_p->m_destruct(m_id_data_p->m_userData);
		delete m_id_data_p;
	}
}


InitDeinit& InitDeinit::operator=(InitDeinit&& a_mM)
{
	InitDeinit_p* this_id_data_p = m_id_data_p;
	m_id_data_p = a_mM.m_id_data_p;
	a_mM.m_id_data_p = this_id_data_p;
	return *this;
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/


InitDeinit_p::InitDeinit_p(const InitDeinit::TypeDestruct& a_destruct, void* a_pData)
    :
      m_destruct(a_destruct?a_destruct:(&StaticDestruct)),
      m_userData(a_pData)
{
}

}  // namespace qtutils{


#endif  // #ifndef QTUTILS_NOT_USE_INITDEINIT
