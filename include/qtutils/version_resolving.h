//
// file:			version_resolving.h
// path:			include/qtutils/version_resolving.h
// created on:		2022 Oct 11
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#ifndef QTUTILS_INCLUDE_QTUTILS_VERSION_RESOLVING_H
#define QTUTILS_INCLUDE_QTUTILS_VERSION_RESOLVING_H


#include <qtutils/export_symbols.h>
#include <cinternal/disable_compiler_warnings.h>
#include <qtutils/disable_utils_warnings.h>
#include <qglobal.h>
#include <cinternal/undisable_compiler_warnings.h>


namespace qtutils{


#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)

#define QMouseEventToGlobalPosition(_mouseEvent_p)    (_mouseEvent_p)->globalPos()

typedef int   sizetype;

#else

#define QMouseEventToGlobalPosition(_mouseEvent_p)    (_mouseEvent_p)->globalPosition().toPoint()

typedef qsizetype   sizetype;

#endif


}  //  namespace qtutils{


#endif  // #ifndef QTUTILS_INCLUDE_QTUTILS_VERSION_RESOLVING_H
