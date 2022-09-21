//
// file:            settings.hpp
// path:			include/qtutils/core/settings.hpp
// created on:		2021 Nov 06
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

//#define QTUTILS_CORE_FORCE_NEW_SETTINGS

#include <qtutils/export_symbols.h>
#include <qtutils/disable_utils_warnings.h>
#include <QSettings>

#if defined(CPPUTILS_POSSIBLE_NO_PERS_FILE) || defined(QTUTILS_CORE_FORCE_NEW_SETTINGS)
#include <QString>
#include <QVariant>
#endif

namespace qtutils{

QTUTILS_EXPORT void InitializeSettings(const QString& mountDirectory);
QTUTILS_EXPORT void CleanupSettings(void);


#if defined(CPPUTILS_POSSIBLE_NO_PERS_FILE) || defined(QTUTILS_CORE_FORCE_NEW_SETTINGS)

class CPPUTILS_DLL_PRIVATE Settings_p;

class QTUTILS_EXPORT Settings final
{
public:
    Settings();
    ~Settings();
    bool contains(const QTUTILS_QT_NSP QString &key) const;
    QTUTILS_QT_NSP QVariant value(const QTUTILS_QT_NSP QString &key, const QTUTILS_QT_NSP QVariant &defaultValue = QTUTILS_QT_NSP QVariant()) const;
    void setValue(const QTUTILS_QT_NSP QString &key, const QTUTILS_QT_NSP QVariant &value);
    void remove(const QTUTILS_QT_NSP QString &key);
	static void setDefaultFormat( QTUTILS_QT_NSP QSettings::Format format);
private:
    Settings_p*const m_set_data;
};

#else
typedef QTUTILS_QT_NSP QSettings Settings;
#endif  // #if defined(CPPUTILS_EMSCRIPTEN_IS_USED) || defined(QTUTILS_CORE_FORCE_NEW_SETTINGS)


}  // namespace qtutils{
