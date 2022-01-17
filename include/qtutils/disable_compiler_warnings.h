//
// file:            disable_compiler_warnings.h
// path:			include/focust/core/disable_compiler_warnings.h
// created on:		2021 Nov 15
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#ifndef FOCUST_INCLUDE_FOCUST_CORE_DISABLE_COMPILER_WARNINGS_H
#define FOCUST_INCLUDE_FOCUST_CORE_DISABLE_COMPILER_WARNINGS_H

#include <focust/core/focust_internal_header.h>

#ifdef _MSC_VER
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#endif


#endif  // #ifndef FOCUST_INCLUDE_FOCUST_CORE_DISABLE_COMPILER_WARNINGS_H
