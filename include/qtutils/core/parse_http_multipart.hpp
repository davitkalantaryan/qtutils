//
// repo:            qtutils
// file:            parse_http_multipart.hpp
// path:			include/qtutils/core/parse_http_multipart.hpp
// created on:		2025 Jun 20
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#pragma once


#include <qtutils/export_symbols.h>
#include <cinternal/disable_compiler_warnings.h>
#include <qtutils/disable_utils_warnings.h>
#include <QByteArray>
#include <QMultiHash>
#include <cinternal/undisable_compiler_warnings.h>


namespace qtutils { namespace core{ namespace httpMPart{


struct SinglePart{
    qsizetype   dataOffset;
    QByteArray  data;
};

typedef QMultiHash<QByteArray, SinglePart >    MultipartMap;

QTUTILS_CORE_EXPORT void ParseMPart(const QByteArray& a_requestBodyBA, const QByteArray& a_boundary, MultipartMap* CPPUTILS_ARG_NN a_map_p);
QTUTILS_CORE_EXPORT QByteArray TakeValueFromPart(const QByteArray& a_part, const QByteArray& a_varName);


}}}  //  namespace qtutils { namespace core{ namespace httpMPart{
