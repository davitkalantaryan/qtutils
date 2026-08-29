//
// repo:            qtutils
// file:            global_functions_02.hpp
// path:			include/qtutils/core/global_functions_02.hpp
// created on:		2026 Aug 29
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//


#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_CORE_GLOBAL_FUNCTIONS_02_HPP
#define QTUTILS_INCLUDE_QTUTILS_CORE_GLOBAL_FUNCTIONS_02_HPP


#include <qtutils/export_symbols.h>
#include <cinternal/disable_compiler_warnings.h>
#include <qtutils/disable_utils_warnings.h>
#include <QByteArray>
#include <cinternal/undisable_compiler_warnings.h>


namespace qtutils { namespace core{ namespace gf02{


template <typename TypeToSrlzDsrlz>
QByteArray Serialize(const TypeToSrlzDsrlz& a_data);
template <typename TypeToSrlzDsrlz>
TypeToSrlzDsrlz Deserialize(const QByteArray& a_dataBS, bool* a_pbOk);


}}}  //  namespace qtutils { namespace core{ namespace gf02{


#ifndef QTUTILS_INCLUDE_QTUTILS_CORE_IMPL_GLOBAL_FUNCTIONS_02_IMPL_HPP
#include <qtutils/core/impl/qtutils_core_global_functions_02.impl.hpp>
#endif


#endif  //  #ifndef QTUTILS_INCLUDE_QTUTILS_CORE_GLOBAL_FUNCTIONS_02_HPP
