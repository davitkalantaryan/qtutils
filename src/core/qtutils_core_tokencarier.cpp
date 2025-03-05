//
// repo:            qtutils
// file:            qtutils_core_tokencarier.cpp
// path:			src/core/qtutils_core_tokencarier.cpp
// created on:		2025 Mar 05
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#include <qtutils/core/tokencarier.hpp>
#include <qtutils/core/parse_jwt.hpp>
#include <cinternal/disable_compiler_warnings.h>
#include <utility>
#include <cinternal/undisable_compiler_warnings.h>


namespace qtutils { namespace core{


TokenCarier::TokenCarier()
{
    m_flags.wr_all = CPPUTILS_BISTATE_MAKE_ALL_BITS_FALSE;
    m_exp = -1;
}


TokenCarier::TokenCarier(const QByteArray& a_token)
{
    m_flags.wr_all = CPPUTILS_BISTATE_MAKE_ALL_BITS_FALSE;
    SetToken(a_token);
}


TokenCarier::TokenCarier(QByteArray&& a_token)
{
    m_flags.wr_all = CPPUTILS_BISTATE_MAKE_ALL_BITS_FALSE;
    SetToken(::std::move(a_token));
}


TokenCarier::TokenCarier(const TokenCarier& a_cM)
{
    m_token = a_cM.m_token;
    m_parseRes = a_cM.m_parseRes;
    m_exp = a_cM.m_exp;
    m_flags.wr_all = a_cM.m_flags.wr_all;
}


TokenCarier::TokenCarier(TokenCarier&& a_mM) noexcept
    :
      m_token(::std::move(a_mM.m_token)),
      m_parseRes(::std::move(a_mM.m_parseRes))
{
    m_exp = a_mM.m_exp;
    m_flags.wr_all = a_mM.m_flags.wr_all;
    a_mM.m_flags.wr.isOk = CPPUTILS_BISTATE_MAKE_BITS_FALSE;
}


TokenCarier& TokenCarier::operator=(const TokenCarier& a_cM)
{
    m_token = a_cM.m_token;
    m_parseRes = a_cM.m_parseRes;
    m_exp = a_cM.m_exp;
    m_flags.wr_all = a_cM.m_flags.wr_all;
    return *this;
}


TokenCarier& TokenCarier::operator=(TokenCarier&& a_mM) noexcept
{
    m_token = ::std::move(a_mM.m_token);
    m_parseRes = ::std::move(a_mM.m_parseRes);
    m_exp = a_mM.m_exp;
    m_flags.wr_all = a_mM.m_flags.wr_all;
    a_mM.m_flags.wr.isOk = CPPUTILS_BISTATE_MAKE_BITS_FALSE;
    return *this;
}


const QByteArray& TokenCarier::token()const noexcept
{
    return m_token;
}


const QList<QJsonObject>& TokenCarier::parseRes()const noexcept
{
    return m_parseRes;
}


qlonglong TokenCarier::expTime()const noexcept
{
    return m_exp;
}


void TokenCarier::SetToken(const QByteArray& a_token)
{
    SetToken(QByteArray(a_token));
}


void TokenCarier::SetToken(QByteArray&& a_token)
{
    m_token = ::std::move(a_token);
    m_parseRes = ParseJWT01(m_token);
    if(m_parseRes.size()<2){
        m_token = QByteArray();
        m_parseRes = QList<QJsonObject>();
        m_exp = -1;
        m_flags.wr.isOk = CPPUTILS_BISTATE_MAKE_BITS_FALSE;
        return;
    }
    
    const QJsonObject& payloadJO = m_parseRes[1];
    m_exp = payloadJO.value("exp").toInteger(-1);
    m_flags.wr.isOk = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
}


uint64_t TokenCarier::isOk()const noexcept
{
    return m_flags.rd.isOk_true;
}


uint64_t TokenCarier::isInvalid()const noexcept
{
    return m_flags.rd.isOk_false;
}



}}  //  namespace qtutils { namespace core{
