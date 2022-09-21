//
// file:            threadorinitdeinit.hpp
// path:			include/qtutils/core/threadorinitdeinit.hpp
// created on:		2022 Feb 23
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once


#include <qtutils/export_symbols.h>

#ifdef CPPUTILS_MULTITHREADED
#include <qtutils/core/threadls.hpp>
#else
#include <qtutils/core/initdeinit.hpp>
#endif

namespace qtutils{

#ifdef CPPUTILS_MULTITHREADED
typedef ThreadLS  ThreadOrInitdeinit;
#else
typedef InitDeinit  ThreadOrInitdeinit;
#endif

}  // namespace qtutils{
