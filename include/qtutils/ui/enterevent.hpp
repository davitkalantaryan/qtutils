//
// file:			enterevent.hpp
// path:			include/qtutils/ui/enterevent.hpp
// created on:		2022 May 29
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/export_symbols.h>
#include <cinternal/disable_compiler_warnings.h>
#include <qtutils/disable_utils_warnings.h>
#include <qglobal.h>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QEvent>
#else
#include <QEnterEvent>
#endif
#include <cinternal/undisable_compiler_warnings.h>


namespace qtutils { namespace ui{

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
typedef QEvent  EnterEvent;
#define qtutils_pos_fnc01   pos
#else
typedef QEnterEvent  EnterEvent;
#define qtutils_pos_fnc01   position().toPoint
#endif

}}  // namespace qtutils { namespace ui{
