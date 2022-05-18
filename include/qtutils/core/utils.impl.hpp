//
// file:            utils.impl.hpp
// path:			include/qtutils/core/utils.impl.hpp
// created on:		2022 May 18
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_CORE_UTILS_IMPL_HPP
#define QTUTILS_INCLUDE_QTUTILS_CORE_UTILS_IMPL_HPP

#ifndef QTUTILS_INCLUDE_QTUTILS_CORE_UTILS_HPP
#include <qtutils/core/utils.hpp>
#endif

#include <QMetaObject>
#include <QMetaEnum>

namespace qtutils { namespace utils {

template <typename EnumType>
QString QtEnumToString(const EnumType& a_value)
{
    Q_STATIC_ASSERT_X(QtPrivate::IsQEnumHelper<EnumType>::Value,
                      "QMetaEnum::fromType only works with enums declared as "
                      "Q_ENUM, Q_ENUM_NS, Q_FLAG or Q_FLAG_NS");

    const QMetaEnum thisEnum = QMetaEnum::fromType<EnumType>();
    return QString(thisEnum.name()) + "::" + thisEnum.key(static_cast<int>(a_value));
}


}}  // namespace qtutils { namespace utils {



#endif  // #ifdef QTUTILS_INCLUDE_QTUTILS_CORE_UTILS_IMPL_HPP
