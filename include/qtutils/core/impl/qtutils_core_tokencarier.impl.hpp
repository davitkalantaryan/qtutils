//
// repo:            qtutils
// file:            tokencarier.hpp
// path:			include/qtutils/core/tokencarier.hpp
// created on:		2025 Mar 05
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_CORE_TOKENCARIER_IMPL_HPP
#define QTUTILS_INCLUDE_QTUTILS_CORE_TOKENCARIER_IMPL_HPP

#ifndef QTUTILS_INCLUDE_QTUTILS_CORE_TOKENCARIER_HPP
#include <qtutils/core/tokencarier.hpp>
#endif


namespace qtutils { namespace core{


template <typename TypeInt>
bool TokenCarier::isOk(const TypeInt& a_currentTime)const noexcept
{
    return (m_exp > static_cast<qlonglong>(a_currentTime));
}


template <typename TypeInt>
bool TokenCarier::isInvalid(const TypeInt& a_currentTime)const noexcept
{
    return (static_cast<qlonglong>(a_currentTime) > m_exp);
}


}}  //  namespace qtutils { namespace core{

#endif  //  #ifndef QTUTILS_INCLUDE_QTUTILS_CORE_TOKENCARIER_IMPL_HPP
