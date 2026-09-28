//
// file:            utils.hpp
// path:			include/qtutils/core/utils.hpp
// created on:		2022 May 18
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_CORE_UTILS_HPP
#define QTUTILS_INCLUDE_QTUTILS_CORE_UTILS_HPP

#include <qtutils/export_symbols.h>
#include <cinternal/disable_compiler_warnings.h>
#include <qtutils/disable_utils_warnings.h>
#include <QString>
#include <cinternal/undisable_compiler_warnings.h>


namespace qtutils { namespace core{ namespace utils {


template <typename EnumType>
QString QtEnumToString(const EnumType& a_value);


}}}  //  namespace qtutils { namespace core{ namespace utils {


#ifndef QTUTILS_INCLUDE_QTUTILS_CORE_UTILS_IMPL_HPP
#include <qtutils/core/impl/qtutils_core_utils.impl.hpp>
#endif


#endif  // #ifdef QTUTILS_INCLUDE_QTUTILS_CORE_UTILS_HPP
