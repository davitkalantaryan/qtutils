//
// repo:            qtutils
// file:            qtutils_core_qtkeychain.cpp
// path:			src/core/qtutils_core_qtkeychain.cpp
// created on:		2026 Aug 29
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//


#include <qtutils/core/qtkeychain.hpp>
#include <cinternal/disable_compiler_warnings.h>
#include <qtutils/core/logger.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QEventLoop>
#include <QStringList>
#include <QSettings>
#include <qt6keychain/keychain.h>
#include <cinternal/undisable_compiler_warnings.h>


namespace qtutils{ namespace core{ namespace qtkeychain{


#define QTUTILS_CORE_KEYCHAINKEYSLIST_PRIVATE_KEY   "qtutils/core/keychain_list_private_key"


static inline void AddKeyIfMissingInline(const QString& a_key){
    QStringList keysList;
    bool bNotFound = true;
    QSettings aSettings;
    if(aSettings.contains(QTUTILS_CORE_KEYCHAINKEYSLIST_PRIVATE_KEY)){
        keysList = aSettings.value(QTUTILS_CORE_KEYCHAINKEYSLIST_PRIVATE_KEY).toStringList();
        const qsizetype keysCount = keysList.size();
        for(qsizetype ind(0); (ind<keysCount) && bNotFound; ++ind){
            if(keysList.at(ind)==a_key){
                bNotFound = false;
                break;
            }
        }  //  for(qsizetype ind(0); (ind<keysCount) && bNotFound; ++ind){
    }  //  if(aSettings.contains(QTUTILS_CORE_KEYCHAINKEYSLIST_PRIVATE_KEY)){
    if(bNotFound){
        keysList.push_back(a_key);
        aSettings.setValue(QTUTILS_CORE_KEYCHAINKEYSLIST_PRIVATE_KEY,keysList);
    }
}


static inline void DeleteDataInline(const QString& a_service, const QString& a_key){
    QKeychain::DeletePasswordJob job(a_service);
    job.setAutoDelete(false);
    job.setKey(a_key);

    QEventLoop loop;
    QObject::connect(&job, &QKeychain::Job::finished, &loop, &QEventLoop::quit);

    job.start();
    loop.exec();

    if (job.error()) {
        QtUtilsDebugLogLvl(1) << "Delete failed:" << job.errorString();
    }
}


QTUTILS_EXPORT bool SaveData(const QString& a_service,const QString& a_key,const QByteArray& a_data)
{
    if(a_data.size()>0){
        AddKeyIfMissingInline(a_key);
    }
    QKeychain::WritePasswordJob job(a_service);
    job.setAutoDelete(false);
    job.setKey(a_key);
    job.setBinaryData(a_data);

    QEventLoop loop;
    QObject::connect(&job, &QKeychain::Job::finished, &loop, &QEventLoop::quit);

    job.start();
    loop.exec();

    if (job.error()) {
        QtUtilsCritical() << "Store failed:" << job.errorString();
        return false;
    }

    return true;
}


QTUTILS_EXPORT QByteArray RetriveData(const QString& a_service,const QString& a_key)
{
    QKeychain::ReadPasswordJob job(a_service);
    job.setAutoDelete(false);
    job.setKey(a_key);

    QEventLoop loop;
    QObject::connect(&job, &QKeychain::Job::finished, &loop, &QEventLoop::quit);

    job.start();
    loop.exec();

    if (job.error()) {
        QtUtilsDebugLogLvl(1) << "Read failed:" << job.errorString();
        return QByteArray();
    }

    const QByteArray plainData = job.binaryData();
    // todo: think whether below lines are needed or not
    if(plainData.size()>0){
        AddKeyIfMissingInline(a_key);
    }

    return plainData;
}


QTUTILS_EXPORT void DeleteData(const QString& a_service, const QString& a_key)
{
    DeleteDataInline(a_service,a_key);
}


QTUTILS_EXPORT void DeleteAllKeys(const QString& a_service)
{
    const QSettings aSettings;
    if(aSettings.contains(QTUTILS_CORE_KEYCHAINKEYSLIST_PRIVATE_KEY)){
        const QStringList keysList = aSettings.value(QTUTILS_CORE_KEYCHAINKEYSLIST_PRIVATE_KEY).toStringList();
        const qsizetype keysCount = keysList.size();
        for(qsizetype ind(0); ind<keysCount; ++ind){
            DeleteDataInline(a_service,keysList.at(ind));
        }  //  for(qsizetype ind(0); ind<keysCount; ++ind){
    }  //  if(aSettings.contains(QTUTILS_CORE_KEYCHAINKEYSLIST_PRIVATE_KEY)){
}


}}}  //  namespace qtutils{ namespace core{ namespace qtkeychain{
