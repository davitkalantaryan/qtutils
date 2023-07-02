//
// file:			internal_header.h
// path:			include/qtutils/internal_header.h
// created on:		2022 Aug 16
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#ifndef INCLUDE_QTUTILS_INTERNAL_HEADER_H
#define INCLUDE_QTUTILS_INTERNAL_HEADER_H

#include <cinternal/internal_header.h>


#if defined(CPPUTILS_EMSCRIPTEN_IS_USED) && !defined(FORCE_USE_STANDARD_DB)
#ifndef QTUTILS_USE_NON_STANDARD_DB
#define QTUTILS_USE_NON_STANDARD_DB
#endif
#endif

#define QTUTILS_QT_NSP  ::

// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=56480
#ifdef CPPUTILS_GCC_BUG_56480
#define QTUTILS_UI_NTDT_NSP         ::
#define QTUTILS_UI_NTDT_NSP_P1
#define QTUTILS_UI_NTDT_NSP_P2
#define QTUTILS_CORE_NTDT_NSP       ::
#define QTUTILS_CORE_NTDT_NSP_P1
#define QTUTILS_CORE_NTDT_NSP_P2
#else
#define QTUTILS_UI_NTDT_NSP         ::qtutils::ui::
#define QTUTILS_UI_NTDT_NSP_P1      namespace qtutils{ namespace ui{
#define QTUTILS_UI_NTDT_NSP_P2      }}
#define QTUTILS_CORE_NTDT_NSP       ::qtutils::
#define QTUTILS_CORE_NTDT_NSP_P1    namespace qtutils{
#define QTUTILS_CORE_NTDT_NSP_P2    }
#endif


#if !defined(QTUTILS_UI_WC_SYSTRY_NEEDED) && !defined(QTUTILS_UI_WC_SYSTRY_NEEDED_ignored)
    #if defined(QTUTILS_UI_WC_SYSTRY_NEEDED_needed) || (!defined(CPPUTILS_EMSCRIPTEN_IS_USED))
        #define QTUTILS_UI_WC_SYSTRY_NEEDED    1
    #endif
#endif


#endif  // #ifndef INCLUDE_QTUTILS_INTERNAL_HEADER_H
