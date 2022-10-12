//
// file:			internal_header.h
// path:			include/qtutils/internal_header.h
// created on:		2022 Aug 16
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#ifndef INCLUDE_QTUTILS_INTERNAL_HEADER_H
#define INCLUDE_QTUTILS_INTERNAL_HEADER_H

#include <cpputils/internal_header.h>


#if defined(CPPUTILS_EMSCRIPTEN_IS_USED) && !defined(FORCE_USE_STANDARD_DB)
#ifndef QTUTILS_USE_NON_STANDARD_DB
#define QTUTILS_USE_NON_STANDARD_DB
#endif
#endif

#define QTUTILS_QT_NSP  ::


#endif  // #ifndef INCLUDE_QTUTILS_INTERNAL_HEADER_H
