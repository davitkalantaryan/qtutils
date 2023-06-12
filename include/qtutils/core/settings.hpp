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


namespace qtutils{


class QTUTILS_EXPORT Settings : public QTUTILS_QT_NSP QSettings
{
public:
    using QTUTILS_QT_NSP QSettings::QSettings;    
#if defined(CPPUTILS_POSSIBLE_NO_PERS_FILE) || defined(QTUTILS_CORE_FORCE_NEW_SETTINGS)
    static void setDefaultFormat(Format a_format);
#endif
};

}  // namespace qtutils{
