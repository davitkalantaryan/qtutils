//
// file:			enterevent.hpp
// path:			include/qtutils/ui/enterevent.hpp
// created on:		2022 May 29
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/qtutils_internal_header.h>
#include <qtutils/disable_utils_warnings.h>
#include <qglobal.h>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QEvent>
#else
#include <QEnterEvent>
#endif


namespace qtutils { namespace ui{

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
typedef QEvent  EnterEvent;
#else
typedef QEnterEvent  EnterEvent;
#endif

}}  // namespace qtutils { namespace ui{
