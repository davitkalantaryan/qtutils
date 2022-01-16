//
// file:			qtutils_internal_header.h
// path:			include/qtutils/qtutils_internal_header.h
// created on:		2022 Jan 16
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#ifndef INCLUDE_QTUTILS_INTERNAL_HEADER_H
#define INCLUDE_QTUTILS_INTERNAL_HEADER_H

#include <stddef.h>

#define qtutils_alloca	alloca

#ifdef _MSC_VER

    #undef qtutils_alloca
    #define qtutils_alloca	_alloca
    #define QTUTILS_UNREACHABLE_CODE(_code)
	//#if _MSC_FULL_VER
	#if (_MSC_VER>1900) // 1900 is VS2015
		#pragma warning (disable:5045)
	#endif
	// assignment within conditional expression (https://docs.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4706?view=msvc-160)
	#pragma warning (disable:4706) 
	// https://docs.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-1-c4711?view=msvc-160
	#pragma warning (disable:4711)
	// Function not inlined althou defined to be inline. 
	// this can happen also with library functions, so this should be disabled
	// https://docs.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-1-c4711?view=msvc-160
	#pragma warning (disable:4710)
    #define QTUTILS_BEFORE_CPP_17_FALL_THR
	#if defined(_MSVC_LANG) && (_MSVC_LANG>=201100L)
        #define QTUTILS_CPP_11_DEFINED		1
	#endif
	#if defined(_MSVC_LANG) && (_MSVC_LANG>=201400L)
        #define QTUTILS_CPP_14_DEFINED		1
	#endif
	#if defined(_MSVC_LANG) && (_MSVC_LANG>=201700L)
        #define QTUTILS_CPP_17_DEFINED		1
	#endif
    #define QTUTILS_DLL_PUBLIC		__declspec(dllexport)
    #define QTUTILS_DLL_PRIVATE
    #define QTUTILS_IMPORT_FROM_DLL	__declspec(dllimport)
    #define QTUTILS_THREAD_LOCAL		__declspec(thread)
	#if !defined(_WIN64) && !defined(_M_ARM64)
		#define CPPUTLS_32_BIT
	#endif
#elif defined(__GNUC__) || defined(__clang__)
    #define QTUTILS_MAY_ALIAS  __attribute__ ((__may_alias__))
    #define QTUTILS_UNREACHABLE_CODE(_code)	_code ;
    #define QTUTILS_BEFORE_CPP_17_FALL_THR	__attribute__ ((fallthrough)) ;
	#if defined(__cplusplus) && (__cplusplus>=201100L)
        #define QTUTILS_CPP_11_DEFINED		1
	#endif
	#if defined(__cplusplus) && (__cplusplus>=201400L)
        #define QTUTILS_CPP_14_DEFINED		1
	#endif
	#if defined(__cplusplus) && (__cplusplus>=201700L)
        #define QTUTILS_CPP_17_DEFINED		1
	#endif
    //#define QTUTILS_DLL_PUBLIC		__attribute__((visibility("default")))
    #define QTUTILS_DLL_PUBLIC
    #define QTUTILS_DLL_PRIVATE		__attribute__((visibility("hidden")))
    #define QTUTILS_IMPORT_FROM_DLL
    #define QTUTILS_THREAD_LOCAL		__thread
#elif defined(__CYGWIN__)
    #define QTUTILS_UNREACHABLE_CODE(_code)	_code ;
    #define QTUTILS_BEFORE_CPP_17_FALL_THR	__attribute__ ((fallthrough)) ;
	#if defined(__cplusplus) && (__cplusplus>=201100L)
        #define QTUTILS_CPP_11_DEFINED		1
	#endif
	#if defined(__cplusplus) && (__cplusplus>=201400L)
        #define QTUTILS_CPP_14_DEFINED		1
	#endif
	#if defined(__cplusplus) && (__cplusplus>=201700L)
        #define QTUTILS_CPP_17_DEFINED		1
	#endif
    #define QTUTILS_DLL_PUBLIC		__attribute__((dllexport))
    #define QTUTILS_DLL_PRIVATE
    #define QTUTILS_IMPORT_FROM_DLL	__attribute__((dllimport))
#elif defined(__MINGW64__) || defined(__MINGW32__)
    #define QTUTILS_UNREACHABLE_CODE(_code)	_code ;
    #define QTUTILS_BEFORE_CPP_17_FALL_THR	__attribute__ ((fallthrough)) ;
	#if defined(__cplusplus) && (__cplusplus>=201100L)
        #define QTUTILS_CPP_11_DEFINED		1
	#endif
	#if defined(__cplusplus) && (__cplusplus>=201400L)
        #define QTUTILS_CPP_14_DEFINED		1
	#endif
	#if defined(__cplusplus) && (__cplusplus>=201700L)
        #define QTUTILS_CPP_17_DEFINED		1
	#endif
    #define QTUTILS_DLL_PUBLIC		_declspec(dllexport)
    #define QTUTILS_DLL_PRIVATE
    #define QTUTILS_IMPORT_FROM_DLL	_declspec(dllimport)
#elif defined(__SUNPRO_CC)
    #define QTUTILS_UNREACHABLE_CODE(_code)	_code ;
    // #define QTUTILS_BEFORE_CPP_17_FALL_THR	__attribute__ ((fallthrough)) ; // ???
    #define QTUTILS_BEFORE_CPP_17_FALL_THR
	#if defined(__cplusplus) && (__cplusplus>=201100L)
        #define QTUTILS_CPP_11_DEFINED		1
	#endif
	#if defined(__cplusplus) && (__cplusplus>=201400L)
        #define QTUTILS_CPP_14_DEFINED		1
	#endif
	#if defined(__cplusplus) && (__cplusplus>=201700L)
        #define QTUTILS_CPP_17_DEFINED		1
	#endif
    #define QTUTILS_DLL_PUBLIC
    #define QTUTILS_DLL_PRIVATE		__hidden
    #define QTUTILS_IMPORT_FROM_DLL
#endif  // #ifdef _MSC_VER

//#undef QTUTILS_CPP_11_DEFINED


//#define QTUTILS_RESTRICT   restrict
#define QTUTILS_RESTRICT

#ifndef QTUTILS_MAY_ALIAS
#define QTUTILS_MAY_ALIAS
#endif

#if defined(QTUTILS_COMPILING_SHARED_LIB)
    #define QTUTILS_EXPORT QTUTILS_DLL_PUBLIC
#elif defined(QTUTILS_USING_STATIC_LIB_OR_OBJECTS)
    #define QTUTILS_EXPORT
#else
    #define QTUTILS_EXPORT QTUTILS_IMPORT_FROM_DLL
#endif

#ifdef __cplusplus
#define QTUTILS_STATIC_CAST(_type,_data)	static_cast<_type>(_data)
#define QTUTILS_GLOBAL	   ::
#define QTUTILS_BEGIN_C   extern "C" {
#define QTUTILS_END_C     }
#define QTUTILS_EXTERN_C  extern "C"
#else
#define QTUTILS_STATIC_CAST(_type,_data)	((_type)(_data))
#define QTUTILS_GLOBAL
#define QTUTILS_BEGIN_C
#define QTUTILS_END_C
#define QTUTILS_EXTERN_C
#endif


#ifdef QTUTILS_CPP_17_DEFINED
#define QTUTILS_FALLTHROUGH	[[fallthrough]] ;
#else
#define QTUTILS_FALLTHROUGH	QTUTILS_BEFORE_CPP_17_FALL_THR
//#define QTUTILS_FALLTHROUGH
#endif

//#undef QTUTILS_CPP_11_DEFINED
//#undef QTUTILS_CPP_14_DEFINED

#ifdef QTUTILS_CPP_11_DEFINED
#define QTUTILS_STD_MOVE_DEFINED   1
#define QTUTILS_NOEXCEPT           noexcept
#define QTUTILS_NULL               nullptr
#define QTUTILS_OVERRIDE           override
//#define QTUTILS_CONSTEXPR          constexpr
//#define QTUTILS_CONSTEXPR_CONS     constexpr
#define QTUTILS_CONSTEXPR          constexpr
#define QTUTILS_CONSTEXPR_FNC          constexpr
#define QTUTILS_CONSTEXPR_EQ(_exp)	= (_exp)
#define QTUTILS_DELETE             =delete;
#define QTUTILS_CONSTEXPR_STACK_ARRAY(_type,_name,_count)	_type _name[_count]
#else
#define QTUTILS_NOEXCEPT	throw()
#define QTUTILS_NULL		NULL
#define QTUTILS_OVERRIDE
#define QTUTILS_CONSTEXPR	const
#define QTUTILS_CONSTEXPR_FNC
#define QTUTILS_CONSTEXPR_EQ(_exp)
#define QTUTILS_DELETE   {}
#define QTUTILS_CONSTEXPR_STACK_ARRAY(_type,_name,_count)	_type * _name = QTUTILS_STATIC_CAST(  _type *,qtutils_alloca((_count)*sizeof(_type))  )
#endif

#ifdef QTUTILS_CPP_14_DEFINED
#define QTUTILS_CONSTEXPR_FNC_CPP14     constexpr
#else
#define QTUTILS_CONSTEXPR_FNC_CPP14
#endif

#ifdef __cplusplus
#define QTUTILS_REGISTER
#else
#define QTUTILS_REGISTER	register
#endif

//#define FOCUST_IS_LITTLE_ENDIAN (((union { unsigned x; unsigned char c; }){1}).c)

#if defined(_DEBUG) || defined(QTUTILS_DEBUG)
// we have debug compilation
#else
// we have release
#ifndef NDEBUG
// let's define NDEBUG (No DEBUG)
#define NDEBUG
#endif
#endif

#ifdef NDEBUG
//#define MAKE_VOID(_val)                 static_cast<void>(_val)
#define DO_DEBUG_EXP(_exp)
#define FOCUST_SAFE_CAST(_type,_val)	static_cast<_type>(_val)
#else
//#define MAKE_VOID(_var)                 do{}while(0)
//#define MAKE_VOID(_var)
#define DO_DEBUG_EXP(_exp)              _exp ;
#define FOCUST_SAFE_CAST(_type,_val)	dynamic_cast<_type>(_val)
#endif


#ifdef __cplusplus
#define FOCUST_STATIC_CAST(_type,_val)    static_cast<_type>(_val)
#else
#define FOCUST_STATIC_CAST(_type,_val)    ( (_type)(_val) )
#endif


#define FOCUST_QT_NSP  ::

#define FOCUST_NO_NULL

#define FOCUST_STRINGIFY(_x)                FOCUST_STRINGIFY_PRIV_RAW(_x)
#define FOCUST_STRINGIFY_PRIV_RAW(_x)		#_x


#endif  // #ifndef INCLUDE_QTUTILS_INTERNAL_HEADER_H
