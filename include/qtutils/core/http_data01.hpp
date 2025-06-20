//
// repo:            qtutils
// file:            http_data01.hpp
// path:			include/qtutils/core/http_data01.hpp
// created on:		2025 Jun 20
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once


#include <qtutils/export_symbols.h>
#include <cinternal/disable_compiler_warnings.h>
#include <qtutils/disable_utils_warnings.h>
#include <QMultiHash>
#include <QList>
#include <QByteArray>
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
#include <QHttpHeaders>
#else
#include <QPair>
#endif
#include <cinternal/undisable_compiler_warnings.h>


namespace qtutils { namespace core{


#if QT_VERSION < QT_VERSION_CHECK(6, 7, 0)
typedef QList<QPair<QByteArray, QByteArray> >   TypeRestHeaders;
#else
typedef QHttpHeaders                            TypeRestHeaders;
#endif
typedef QList<QByteArray>                       ByteArrayList;
typedef QMultiHash<QByteArray, QByteArray>      HeadersMap;


}}  // namespace qtutils { namespace core{
