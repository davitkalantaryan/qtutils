//
// repo:            qtutils
// file:            qtutils_core_versionnumber.cpp
// path:			src/core/focust_websockscontainer.cpp
// created on:		2022 Jan 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#include <qtutils/core/versionnumber.hpp>


namespace qtutils { namespace core{


VersionNumber::~VersionNumber()
{
    delete m_vn_data_p;
}


VersionNumber::VersionNumber()
    :
      m_vn_data_p(new QVersionNumber())
{
}


VersionNumber::VersionNumber(const QVersionNumber& a_qvn)
    :
      m_vn_data_p(new QVersionNumber(a_qvn.segments()))
{
}


VersionNumber::VersionNumber(const VersionNumber& a_cM)
    :
      m_vn_data_p(new QVersionNumber(a_cM.m_vn_data_p->segments()))
{
}


VersionNumber::VersionNumber(VersionNumber&& a_mM)
    :
      m_vn_data_p(a_mM.m_vn_data_p)
{
    a_mM.m_vn_data_p = nullptr;
}


VersionNumber& VersionNumber::operator=(const VersionNumber& a_cM)
{
    delete m_vn_data_p;
    m_vn_data_p = new QVersionNumber(a_cM.m_vn_data_p->segments());
    return *this;
}


VersionNumber& VersionNumber::operator=(VersionNumber&& a_mM)
{
    QVersionNumber*const pThisQVN = m_vn_data_p;
    m_vn_data_p = a_mM.m_vn_data_p;
    a_mM.m_vn_data_p = pThisQVN;
    return *this;
}


void VersionNumber::CopyFrom(const VersionNumber& a_cM)
{
    delete m_vn_data_p;
    m_vn_data_p = new QVersionNumber(a_cM.m_vn_data_p->segments());
}


void VersionNumber::ReplaceWithOther(VersionNumber* CPPUTILS_ARG_NN a_pmM)
{
    QVersionNumber*const pThisQVN = m_vn_data_p;
    m_vn_data_p = a_pmM->m_vn_data_p;
    a_pmM->m_vn_data_p = pThisQVN;
}


QVersionNumber* VersionNumber::qVersionNumber()const
{
    return m_vn_data_p;
}


QVersionNumber* VersionNumber::operator->()const
{
    return m_vn_data_p;
}


const QVersionNumber& VersionNumber::operator*()const
{
    return *m_vn_data_p;
}


}}  //  namespace qtutils { namespace core{
