//
// file:            qtutils_settings.cpp
// path:			src/core/qtutils_settings.cpp
// created on:		2021 Nov 06
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#include <qtutils/core/settings.hpp>
#if defined(CPPUTILS_POSSIBLE_NO_PERS_FILE) || defined(QTUTILS_CORE_FORCE_NEW_SETTINGS)
#include <stdlib.h>
#include <qtutils/disable_utils_warnings.h>
#include <QIODevice>
#endif


namespace qtutils{


Settings::~Settings()
{
    //
}


#if defined(CPPUTILS_POSSIBLE_NO_PERS_FILE) || defined(QTUTILS_CORE_FORCE_NEW_SETTINGS)

static bool ReadXmlFileStatic(QIODevice& a_device, QSettings::SettingsMap& a_map)
{
    static_cast<void>(a_device);
    static_cast<void>(a_map);
    return true;
}


static bool WriteXmlFileStatic(QIODevice& a_device, const QSettings::SettingsMap& a_map)
{
    static_cast<void>(a_device);
    static_cast<void>(a_map);
    return true;
}

#endif  //  #if defined(CPPUTILS_POSSIBLE_NO_PERS_FILE) || defined(QTUTILS_CORE_FORCE_NEW_SETTINGS)

}  // namespace qtutils{

#if defined(CPPUTILS_POSSIBLE_NO_PERS_FILE) || defined(QTUTILS_CORE_FORCE_NEW_SETTINGS)

CPPUTILS_CODE_INITIALIZER(qtutils_core_settings_emscripten_init){
    QSettings::registerFormat("xml", &::qtutils::ReadXmlFileStatic, &::qtutils::WriteXmlFileStatic);
    QSettings::registerFormat("ini", &::qtutils::ReadXmlFileStatic, &::qtutils::WriteXmlFileStatic);
}

#endif  //  #if defined(CPPUTILS_POSSIBLE_NO_PERS_FILE) || defined(QTUTILS_CORE_FORCE_NEW_SETTINGS)
