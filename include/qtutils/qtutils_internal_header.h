//
// file:			qtutils_internal_header.h
// path:			include/qtutils/qtutils_internal_header.h
// created on:		2022 Jan 16
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#ifndef INCLUDE_QTUTILS_INTERNAL_HEADER_H
#define INCLUDE_QTUTILS_INTERNAL_HEADER_H

#include <cpputils_internal_header.h>


#if defined(QTUTILS_COMPILING_SHARED_LIB)
    #define QTUTILS_EXPORT CPPUTILS_DLL_PUBLIC
#elif defined(QTUTILS_USING_STATIC_LIB_OR_OBJECTS)
    #define QTUTILS_EXPORT
#else
    #define QTUTILS_EXPORT CPPUTILS_IMPORT_FROM_DLL
#endif


#endif  // #ifndef INCLUDE_QTUTILS_INTERNAL_HEADER_H
