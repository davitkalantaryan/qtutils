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
#include <QCoreApplication>
#include <QThread>
#include <QMetaObject>
#include <qt6keychain/keychain.h>
#include <cinternal/undisable_compiler_warnings.h>


namespace qtutils{ namespace core{ namespace qtkeychain{


#define QTUTILS_CORE_KEYCHAINKEYSLIST_PRIVATE_KEY   "qtutils/core/keychain_list_private_key"


static inline void AddKeyIfMissingInlineRaw(const QString& a_key){
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


static inline void RemoveKeyIfExistsRaw(const QString& a_key){
    QSettings aSettings;
    if(aSettings.contains(QTUTILS_CORE_KEYCHAINKEYSLIST_PRIVATE_KEY)){
        bool bFound = false;
        QStringList keysList = aSettings.value(QTUTILS_CORE_KEYCHAINKEYSLIST_PRIVATE_KEY).toStringList();
        const qsizetype keysCount = keysList.size();
        for(qsizetype ind(0); ind<keysCount; ++ind){
            if(keysList.at(ind)==a_key){
                keysList.remove(ind);
                bFound = true;
                break;
            }
        }  //  for(qsizetype ind(0); (ind<keysCount) && bNotFound; ++ind){
        if(bFound){
            aSettings.setValue(QTUTILS_CORE_KEYCHAINKEYSLIST_PRIVATE_KEY,keysList);
        }
    }  //  if(aSettings.contains(QTUTILS_CORE_KEYCHAINKEYSLIST_PRIVATE_KEY)){
}


static inline void DeleteDataInlineRaw(const QString& a_service, const QString& a_key){
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


static inline bool SaveDataAppInAppThreadInline(const QString& a_service,const QString& a_key,const QByteArray& a_data){
    if(a_data.size()>0){
        AddKeyIfMissingInlineRaw(a_key);
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


static inline QByteArray RetriveDataInAppThreadInline(const QString& a_service,const QString& a_key){
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
        AddKeyIfMissingInlineRaw(a_key);
    }

    return plainData;
}


static inline void DeleteDataInAppThreadInline(const QString& a_service, const QString& a_key){
    DeleteDataInlineRaw(a_service,a_key);
    RemoveKeyIfExistsRaw(a_key);
}


static inline void DeleteAllKeysInAppThreadInline(const QString& a_service){
    QSettings aSettings;
    if(aSettings.contains(QTUTILS_CORE_KEYCHAINKEYSLIST_PRIVATE_KEY)){
        const QStringList keysList = aSettings.value(QTUTILS_CORE_KEYCHAINKEYSLIST_PRIVATE_KEY).toStringList();
        const qsizetype keysCount = keysList.size();
        for(qsizetype ind(0); ind<keysCount; ++ind){
            DeleteDataInlineRaw(a_service,keysList.at(ind));
        }  //  for(qsizetype ind(0); ind<keysCount; ++ind){
        aSettings.remove(QTUTILS_CORE_KEYCHAINKEYSLIST_PRIVATE_KEY);
    }  //  if(aSettings.contains(QTUTILS_CORE_KEYCHAINKEYSLIST_PRIVATE_KEY)){
}


QTUTILS_EXPORT bool SaveData(const QString& a_service,const QString& a_key,const QByteArray& a_data)
{
    QCoreApplication* const pCoreApp = qApp;
    if(pCoreApp){
        const QThread* const appThread = pCoreApp->thread();
        const QThread* const thisThread = QThread::currentThread();
        if(appThread==thisThread){
            return SaveDataAppInAppThreadInline(a_service,a_key,a_data);
        }  //  if(appThread==thisThread){
        else{
            //bool bReturn;
            QMetaObject::invokeMethod(pCoreApp,[a_service,a_key,a_data](){
                SaveDataAppInAppThreadInline(a_service,a_key,a_data);
            }/*,Qt::BlockingQueuedConnection*/);
            return true;
        }  //  else of 'if(appThread==thisThread){'
    }  //  if(pCoreApp){

    return false;
}


QTUTILS_EXPORT QByteArray RetriveData(const QString& a_service,const QString& a_key)
{
    QCoreApplication* const pCoreApp = qApp;
    if(pCoreApp){
        const QThread* const appThread = pCoreApp->thread();
        const QThread* const thisThread = QThread::currentThread();
        if(appThread==thisThread){
            return RetriveDataInAppThreadInline(a_service,a_key);
        }  //  if(appThread==thisThread){
        else{
            QByteArray plainData;
            QMetaObject::invokeMethod(pCoreApp,[&a_service,&a_key,&plainData](){
                plainData = RetriveDataInAppThreadInline(a_service,a_key);
            },Qt::BlockingQueuedConnection);
            return plainData;
        }  //  else of 'if(appThread==thisThread){'
    }  //  if(pCoreApp){

    return QByteArray();
}


QTUTILS_EXPORT void DeleteData(const QString& a_service, const QString& a_key)
{
    QCoreApplication* const pCoreApp = qApp;
    if(pCoreApp){
        const QThread* const appThread = pCoreApp->thread();
        const QThread* const thisThread = QThread::currentThread();
        if(appThread==thisThread){
            DeleteDataInAppThreadInline(a_service,a_key);
        }  //  if(appThread==thisThread){
        else{
            QMetaObject::invokeMethod(pCoreApp,[a_service,a_key](){
                DeleteDataInAppThreadInline(a_service,a_key);
            }/*,Qt::BlockingQueuedConnection*/);
        }  //  else of 'if(appThread==thisThread){'
    }  //  if(pCoreApp){
}


QTUTILS_EXPORT void DeleteAllKeys(const QString& a_service)
{
    QCoreApplication* const pCoreApp = qApp;
    if(pCoreApp){
        const QThread* const appThread = pCoreApp->thread();
        const QThread* const thisThread = QThread::currentThread();
        if(appThread==thisThread){
            DeleteAllKeysInAppThreadInline(a_service);
        }  //  if(appThread==thisThread){
        else{
            QMetaObject::invokeMethod(pCoreApp,[a_service](){
                DeleteAllKeysInAppThreadInline(a_service);
            }/*,Qt::BlockingQueuedConnection*/);
        }  //  else of 'if(appThread==thisThread){'
    }  //  if(pCoreApp){
}


}}}  //  namespace qtutils{ namespace core{ namespace qtkeychain{
