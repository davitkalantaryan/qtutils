//
// file:            invokeblocked.hpp
// path:			include/qtutils/core/invokeblocked.hpp
// created on:		2022 Apr 30
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#ifndef QTUTILS_INCLUDE_QTUTILS_CORE_INVOKEBLOCKED_HPP
#define QTUTILS_INCLUDE_QTUTILS_CORE_INVOKEBLOCKED_HPP

#include <qtutils/export_symbols.h>
#include <functional>
#include <qtutils/disable_utils_warnings.h>
#include <QObject>

namespace qtutils {


template<typename... Targs>
bool invokeMethodBlocked(QObject* a_pObj, const ::std::function<void(Targs...)>& a_fnc, Targs... a_args);
QTUTILS_EXPORT bool invokeMethodBlocked(QObject* a_pObj, const ::std::function<void(void)>& a_fnc);


}  // namespace qtutils {


#ifndef QTUTILS_INCLUDE_QTUTILS_CORE_INVOKEBLOCKED_IMPL_HPP
#include "invokeblocked.impl.hpp"
#endif


#endif  // #ifndef QTUTILS_INCLUDE_QTUTILS_CORE_INVOKEBLOCKED_HPP
