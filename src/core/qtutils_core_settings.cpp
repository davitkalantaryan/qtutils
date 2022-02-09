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
#include <QFileInfo>
#include <QDir>


namespace qtutils{


class CPPUTILS_DLL_PRIVATE Settings_p final
{
public:
    QSettings::SettingsMap     m_rawSettings;
};


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

static ::std::mutex s_mapMutex;
static QString      s_defaultFilePath;
static bool         s_bIsSynchron = false;
static QSettings::SettingsMap*  s_pMap = nullptr;

static void qtutils_fs_sync_static(void*);


Settings::Settings()
    :
      m_set_data(new Settings_p())
{
    ::std::lock_guard<::std::mutex> aGuard(s_mapMutex);

    if(s_bIsSynchron){
        //QtUtilsDebug()<<"s_defaultFilePath:"<<s_defaultFilePath;
        QFile aFile(s_defaultFilePath);

        if(!aFile.open(QIODevice::ReadOnly)){
            QtUtilsCritical()<<"unable to open settings file for read: "<<s_defaultFilePath;
            return;
        }

        QDataStream out(&aFile);

        out>>(m_set_data->m_rawSettings);
    }
    else{
        m_set_data->m_rawSettings = *s_pMap;
    }
}


Settings::~Settings()
{
    ::std::lock_guard<::std::mutex> aGuard(s_mapMutex);

    if(s_bIsSynchron){
        QFile aFile(s_defaultFilePath);

        if(!aFile.open(QIODevice::WriteOnly|QIODevice::Truncate)){
            QtUtilsCritical()<<"unable to open settings file for write: "<<s_defaultFilePath;
            delete m_set_data;
            return;
        }

        QDataStream out(&aFile);

        out<<(m_set_data->m_rawSettings);
        ::cpputils::emscripten::fs_sync();
    }
    else{
        s_pMap->insert(m_set_data->m_rawSettings);
    }

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


QTUTILS_EXPORT void InitializeSettings(const QString& a_mountDirectory)
{
    s_pMap = new QSettings::SettingsMap();

    if(QDir(a_mountDirectory).exists()){
        QtUtilsDebug()<<"Directory " + a_mountDirectory + " already mounted";
    }
    else{
        QtUtilsDebug()<<"Mounting idbfs file system in the directory: " + a_mountDirectory;
        ::cpputils::emscripten::mount_idbfs_file_system(a_mountDirectory.toUtf8(),&qtutils_fs_sync_static,nullptr);
    }

    const QString fileName = QCoreApplication::applicationName()+".ini";
    const QString directoryPath =  QFileInfo(a_mountDirectory,QCoreApplication::organizationName()).filePath();
    if(QDir(directoryPath).exists()){
        QtUtilsDebug()<<"Directory " + directoryPath + " already created";
    }
    else{
        QtUtilsDebug()<<"Creating the directory: " + directoryPath;
        QDir(a_mountDirectory).mkdir(directoryPath);
    }
    s_defaultFilePath  = QFileInfo(directoryPath,fileName).filePath();

    QtUtilsDebug()<<"s_defaultFilePath:"<<s_defaultFilePath;

    QSettings::setPath(QSettings::IniFormat,QSettings::SystemScope,s_defaultFilePath);
    QSettings::setPath(QSettings::IniFormat,QSettings::UserScope,s_defaultFilePath);
    QSettings::setDefaultFormat(QSettings::IniFormat);

    // open once the file
    QFile aFile(s_defaultFilePath);
    if(!aFile.open(QIODevice::ReadWrite)){
        QtUtilsCritical()<<"unable to open settings file for initing: "<<s_defaultFilePath;
        return;
    }
}


QTUTILS_EXPORT void CleanupSettings(void)
{
    delete s_pMap;
    s_pMap = nullptr;
}



static void qtutils_fs_sync_static(void*)
{
    QtUtilsDebug()<<"!!!!!!! FS sync called";
    ::std::lock_guard<::std::mutex> aGuard(s_mapMutex);
    if(s_bIsSynchron){return;}
    s_bIsSynchron = true;
    QFile aFile(s_defaultFilePath);

    if(!aFile.open(QIODevice::WriteOnly|QIODevice::Truncate)){
        QtUtilsCritical()<<"unable to open settings file for write: "<<s_defaultFilePath;
        return;
    }

    QDataStream out(&aFile);

    out<<(*s_pMap);
    ::cpputils::emscripten::fs_sync();
}



}  // namespace qtutils{


#else

namespace qtutils{

QTUTILS_EXPORT void InitializeSettings(const QString&)
{
}


QTUTILS_EXPORT void CleanupSettings(void)
{
    //
}

}  // namespace qtutils{


#endif  // #if defined(CPPUTILS_POSSIBLE_NO_PERS_FILE) || defined(QTUTILS_CORE_FORCE_NEW_SETTINGS)

