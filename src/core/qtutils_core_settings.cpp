//
// file:            qtutils_settings.cpp
// path:			src/core/qtutils_settings.cpp
// created on:		2021 Nov 06
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#include <qtutils/core/settings.hpp>
#if defined(CPPUTILS_POSSIBLE_NO_PERS_FILE) || defined(QTUTILS_CORE_FORCE_NEW_SETTINGS)
#include <qtutils/core/logger.hpp>
#include <cpputils/hashtbl.hpp>
#include <mutex>
#include <qtutils/disable_utils_warnings.h>
#include <cpputils/emscripten.hpp>
#include <QMetaObject>
#include <QThread>
#include <QCoreApplication>
#include <QFile>
#include <QVariantMap>
#include <QDataStream>


namespace qtutils{

#define FOCUST_INDEXDB_MOUNT_POINT  "/focust_idbfs"


static bool WriteFuncStatic(QIODevice& device, const QSettings::SettingsMap&map);
static bool ReadFuncStatic(QIODevice& device, QSettings::SettingsMap &map);


class CPPUTILS_DLL_PRIVATE Settings_p final
{
public:
    QSettings::SettingsMap     m_rawSettings;
};


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/


Settings::Settings()
    :
      m_set_data(new Settings_p())
{
    const QString settingsFilePath =     FOCUST_INDEXDB_MOUNT_POINT "/" +QCoreApplication::applicationName()+".ini";
    QFile aFile(settingsFilePath);

    if(!aFile.open(QIODevice::ReadOnly)){
        QtUtilsCritical()<<"unable to open settings file for read: "<<settingsFilePath;
        return;
    }

    QDataStream out(&aFile);

    out>>(m_set_data->m_rawSettings);
}


Settings::~Settings()
{
    const QString settingsFilePath =     FOCUST_INDEXDB_MOUNT_POINT "/" +QCoreApplication::applicationName()+".ini";
    QFile aFile(settingsFilePath);

    if(!aFile.open(QIODevice::WriteOnly|QIODevice::Truncate)){
        QtUtilsCritical()<<"unable to open settings file for write: "<<settingsFilePath;
        return;
    }

    QDataStream out(&aFile);

    out<<(m_set_data->m_rawSettings);
    ::cpputils::emscripten::fs_sync();
    delete m_set_data;
}


bool Settings::contains(const QString& a_key)const
{
    return m_set_data->m_rawSettings.contains(a_key);
}

QVariant Settings::value(const QString & a_key, const QVariant & a_defaultValue) const
{
    return m_set_data->m_rawSettings.value(a_key,a_defaultValue);
}

void Settings::setValue(const QString &a_key, const QVariant &a_value)
{
    m_set_data->m_rawSettings.insert(a_key,a_value);
}


void Settings::setDefaultFormat( QTUTILS_QT_NSP QSettings::Format)
{
    QSettings::setDefaultFormat(QSettings::IniFormat);
}


QTUTILS_EXPORT void InitializeSettings(void)
{
    ::cpputils::emscripten::mount_idbfs_file_system(FOCUST_INDEXDB_MOUNT_POINT);
    QSettings::Format  aFormat = QSettings::registerFormat("ini",&ReadFuncStatic,&WriteFuncStatic);
    if(aFormat==QSettings::InvalidFormat){
        QtUtilsCritical()<<"Unable to regsiter settings format";
    }

    const QString settingsFilePath =     FOCUST_INDEXDB_MOUNT_POINT "/" +QCoreApplication::applicationName()+".ini";

    QSettings::setPath(QSettings::IniFormat,QSettings::SystemScope,settingsFilePath);
    QSettings::setPath(QSettings::IniFormat,QSettings::UserScope,settingsFilePath);
    QSettings::setDefaultFormat(QSettings::IniFormat);
}


QTUTILS_EXPORT void CleanupSettings(void)
{
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

static bool WriteFuncStatic(QIODevice&, const QSettings::SettingsMap& a_map)
{
    const QString settingsFilePath =     FOCUST_INDEXDB_MOUNT_POINT "/" +QCoreApplication::applicationName()+".ini";
    QFile aFile(settingsFilePath);

    if(!aFile.open(QIODevice::WriteOnly|QIODevice::Truncate)){
        QtUtilsCritical()<<"unable to open settings file for write: "<<settingsFilePath;
        return false;
    }

    QDataStream out(&aFile);

    out<<a_map;
    return true;
}


static bool ReadFuncStatic(QIODevice&, QSettings::SettingsMap& a_map)
{
    const QString settingsFilePath =     FOCUST_INDEXDB_MOUNT_POINT "/" +QCoreApplication::applicationName()+".ini";
    QFile aFile(settingsFilePath);

    if(!aFile.open(QIODevice::ReadOnly)){
        QtUtilsCritical()<<"unable to open settings file for read: "<<settingsFilePath;
        return false;
    }

    QDataStream out(&aFile);

    out>>a_map;
    ::cpputils::emscripten::fs_sync();
    return true;
}



}  // namespace qtutils{


#else

namespace qtutils{

QTUTILS_EXPORT void InitializeSettings(void)
{
}


QTUTILS_EXPORT void CleanupSettings(void)
{
    //
}

}  // namespace qtutils{


#endif  // #if defined(CPPUTILS_POSSIBLE_NO_PERS_FILE) || defined(QTUTILS_CORE_FORCE_NEW_SETTINGS)

