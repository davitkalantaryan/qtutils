//
// repo:            qtutils
// file:            tokencarier.hpp
// path:			include/qtutils/core/tokencarier.hpp
// created on:		2025 Mar 05
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_CORE_TOKENCARIER_HPP
#define QTUTILS_INCLUDE_QTUTILS_CORE_TOKENCARIER_HPP

#include <qtutils/export_symbols.h>
#include <cinternal/bistateflags.h>
#include <cinternal/disable_compiler_warnings.h>
#include <qtutils/disable_utils_warnings.h>
#include <QByteArray>
#include <QList>
#include <QJsonObject>
#include <cinternal/undisable_compiler_warnings.h>


namespace qtutils { namespace core{


class QTUTILS_CORE_EXPORT TokenCarier
{
public:
    TokenCarier();
    TokenCarier(const QByteArray& a_token);
    TokenCarier(QByteArray&& a_token) ;
    TokenCarier(const TokenCarier& a_cM);
    TokenCarier(TokenCarier&& a_mM) noexcept;
    
    TokenCarier& operator=(const TokenCarier& a_cM);
    TokenCarier& operator=(TokenCarier&& a_mM) noexcept;
    
    const QByteArray& token()const noexcept;
    const QList<QJsonObject>& parseRes()const noexcept;
    qlonglong expTime()const noexcept;
    uint64_t isOk()const noexcept;
    uint64_t isInvalid()const noexcept;
    void Reset() noexcept;
    void SetToken(const QByteArray& a_token);
    void SetToken(QByteArray&& a_token);

    template <typename TypeInt>
    bool isOk(const TypeInt& a_currentTime)const noexcept;
    template <typename TypeInt>
    bool isInvalid(const TypeInt& a_currentTime)const noexcept;
    
private:
    QByteArray          m_token;
    QList<QJsonObject>  m_parseRes;
    qlonglong           m_exp;
    CPPUTILS_BISTATE_FLAGS_UN(
        isOk
    )m_flags;
};


}}  //  namespace qtutils { namespace core{


#ifndef QTUTILS_INCLUDE_QTUTILS_CORE_TOKENCARIER_IMPL_HPP
#include <qtutils/core/tokencarier.impl.hpp>
#endif


#endif  //  #ifndef QTUTILS_INCLUDE_QTUTILS_CORE_TOKENCARIER_HPP
