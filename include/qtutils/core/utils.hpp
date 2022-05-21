//
// file:            utils.hpp
// path:			include/qtutils/core/utils.hpp
// created on:		2022 May 18
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_CORE_UTILS_HPP
#define QTUTILS_INCLUDE_QTUTILS_CORE_UTILS_HPP

#include <qtutils/qtutils_internal_header.h>
#include <qtutils/disable_utils_warnings.h>
#include <QString>

namespace qtutils { namespace utils {

template <typename EnumType>
QString QtEnumToString(const EnumType& a_value);

}}  // namespace qtutils { namespace utils {


#ifndef QTUTILS_INCLUDE_QTUTILS_CORE_UTILS_IMPL_HPP
#include "utils.impl.hpp"
#endif


#endif  // #ifdef QTUTILS_INCLUDE_QTUTILS_CORE_UTILS_HPP
