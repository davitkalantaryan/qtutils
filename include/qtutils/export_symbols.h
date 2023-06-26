//
// file:			export_symbols.h
// path:			include/qtutils/export_symbols.h
// created on:		2022 Sep 21
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#ifndef INCLUDE_QTUTILS_EXPORT_SYMBOLS_H
#define INCLUDE_QTUTILS_EXPORT_SYMBOLS_H

#include <qtutils/internal_header.h>


#ifndef QTUTILS_EXPORT
#if defined(QTUTILS_COMPILING_SHARED_LIB)
    #define QTUTILS_EXPORT CPPUTILS_DLL_PUBLIC
#elif defined(QTUTILS_USING_STATIC_LIB_OR_OBJECTS)
    #define QTUTILS_EXPORT
#elif defined(QTUTILS_LOAD_FROM_DLL)
	#define QTUTILS_EXPORT CPPUTILS_IMPORT_FROM_DLL
#else
    #define QTUTILS_EXPORT CPPUTILS_DLL_PRIVATE
#endif
#endif


#ifndef QTUTILS_CORE_EXPORT
#define QTUTILS_CORE_EXPORT QTUTILS_EXPORT
#endif


#endif  // #ifndef INCLUDE_QTUTILS_EXPORT_SYMBOLS_H
