//
// file:            settings.hpp
// path:			include/qtutils/core/settings.hpp
// created on:		2021 Nov 06
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

//#define FOCUST_EMSCRIPTEN_IS_USED

#include <qtutils/qtutils_internal_header.h>
#include <qtutils/disable_utils_warnings.h>
#include <QSettings>

#ifdef CPPUTILS_EMSCRIPTEN_IS_USED
#include <QString>
#include <QVariant>
#endif

namespace qtutils{

QTUTILS_EXPORT void InitializeSettings(QTUTILS_QT_NSP QSettings::Format format);
QTUTILS_EXPORT void CleanupSettings(void);


#ifdef CPPUTILS_EMSCRIPTEN_IS_USED

class QTUTILS_EXPORT Settings final
{
public:
    bool contains(const QTUTILS_QT_NSP QString &key) const;
    QTUTILS_QT_NSP QVariant value(const QTUTILS_QT_NSP QString &key, const QTUTILS_QT_NSP QVariant &defaultValue = QTUTILS_QT_NSP QVariant()) const;
    void setValue(const QTUTILS_QT_NSP QString &key, const QTUTILS_QT_NSP QVariant &value);
	static void setDefaultFormat( QTUTILS_QT_NSP QSettings::Format format);
};

#else
typedef QTUTILS_QT_NSP QSettings Settings;
#endif  // #ifdef CPPUTILS_EMSCRIPTEN_IS_USED


}  // namespace qtutils{
