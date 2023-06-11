//
// file:            qtutils_settings.cpp
// path:			src/core/qtutils_settings.cpp
// created on:		2021 Nov 06
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#include <qtutils/core/settings.hpp>

namespace qtutils{


#if defined(CPPUTILS_POSSIBLE_NO_PERS_FILE) || defined(QTUTILS_CORE_FORCE_NEW_SETTINGS)


void Settings::setDefaultFormat(Format a_format)
{
    switch(a_format){
    case QSettings::IniFormat:
        break;
    default:
        QSettings::setDefaultFormat(a_format);
        break;
    }  //  switch(a_format){
}


#endif  //  #if defined(CPPUTILS_POSSIBLE_NO_PERS_FILE) || defined(QTUTILS_CORE_FORCE_NEW_SETTINGS)

}  // namespace qtutils{
