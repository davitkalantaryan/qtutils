//
// file:            qtutils_settings.cpp
// path:			src/core/qtutils_settings.cpp
// created on:		2021 Nov 06
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#include <qtutils/core/settings.hpp>
#if defined(CPPUTILS_POSSIBLE_NO_PERS_FILE) || defined(QTUTILS_CORE_FORCE_NEW_SETTINGS)
#include <cpputils/hashtbl.hpp>
#include <mutex>
#include <qtutils/disable_utils_warnings.h>
#include <QMetaObject>
#include <QThread>


namespace qtutils{

static ::cpputils::hashtbl::Base<QString,QVariant>  s_values;
static ::std::mutex                                 s_mutex;
static QSettings*                                   s_pSettings = nullptr;


static inline void EmptyAllPairsFromMapNoLock(void)
{
    ::cpputils::hashtbl::Base<QString,QVariant>::const_iterator iterTmp, iter=s_values.begin();
    while(iter!=::cpputils::hashtbl::Base<QString,QVariant>::s_endIter){
        s_pSettings->setValue(iter->first,iter->second);
        iterTmp = iter;
        ++iterTmp;
        s_values.RemoveEntry(iter);
        iter = iterTmp;
    }
}


bool Settings::contains(const QString & a_key)const
{
    ::std::lock_guard< ::std::mutex> aGualrd(s_mutex);
    
    if(s_pSettings){
        bool bSettingsOk = false;
        bool bContains = false;
        const Qt::ConnectionType callCon = QThread::currentThread()==s_pSettings->thread()?Qt::AutoConnection:Qt::BlockingQueuedConnection;
        QMetaObject::invokeMethod(s_pSettings,[a_key,&bSettingsOk,&bContains](){
            if (s_pSettings->status() == QSettings::NoError) {
                bSettingsOk = true;
                EmptyAllPairsFromMapNoLock();
                bContains = s_pSettings->contains(a_key);
            }
        },callCon);
        if(bSettingsOk){
            return bContains;
        }
    }
    
    ::cpputils::hashtbl::Base<QString,QVariant>::const_iterator iter;
    iter = s_values.FindEntry(a_key);
    return iter?true:false;
}

QVariant Settings::value(const QString & a_key, const QVariant & a_defaultValue) const
{
    ::std::lock_guard< ::std::mutex> aGualrd(s_mutex);
    
    if(s_pSettings){
        bool bSettingsOk = false;
        QVariant retValue;
        const Qt::ConnectionType callCon = QThread::currentThread()==s_pSettings->thread()?Qt::AutoConnection:Qt::BlockingQueuedConnection;
        QMetaObject::invokeMethod(s_pSettings,[a_key,a_defaultValue,&bSettingsOk,&retValue](){
            if (s_pSettings->status() == QSettings::NoError) {
                bSettingsOk = true;
                EmptyAllPairsFromMapNoLock();
                retValue = s_pSettings->value(a_key,a_defaultValue);
            }
        },callCon);
        if(bSettingsOk){
            return retValue;
        }
    }
    
    ::cpputils::hashtbl::Base<QString,QVariant>::const_iterator iter;
    iter = s_values.FindEntry(a_key);
    if(iter){
        return iter->second;
    }
    return a_defaultValue;
}

void Settings::setValue(const QString &a_key, const QVariant &a_value)
{
    ::std::lock_guard< ::std::mutex> aGualrd(s_mutex);
    
    if(s_pSettings){
        bool bSettingsOk = false;
        const Qt::ConnectionType callCon = QThread::currentThread()==s_pSettings->thread()?Qt::AutoConnection:Qt::BlockingQueuedConnection;
        QMetaObject::invokeMethod(s_pSettings,[a_key,a_value,&bSettingsOk](){
            if (s_pSettings->status() == QSettings::NoError) {
                bSettingsOk = true;
                EmptyAllPairsFromMapNoLock();
                s_pSettings->setValue(a_key,a_value);
            }
        },callCon);
        if(bSettingsOk){
            return;
        }
    }
    
    s_values.AddOrReplaceEntry(a_key,a_value);
}


void Settings::setDefaultFormat( QTUTILS_QT_NSP QSettings::Format a_format)
{
    ::std::lock_guard< ::std::mutex> aGualrd(s_mutex);
    if(s_pSettings){
        const Qt::ConnectionType callCon = QThread::currentThread()==s_pSettings->thread()?Qt::AutoConnection:Qt::BlockingQueuedConnection;
        QMetaObject::invokeMethod(s_pSettings,[](){
            if (s_pSettings->status() == QSettings::NoError) {
                EmptyAllPairsFromMapNoLock();
            }
            delete s_pSettings;
            s_pSettings = nullptr;
        },callCon);
    }
	QSettings::setDefaultFormat(a_format);
    s_pSettings = new QSettings();
}


QTUTILS_EXPORT void InitializeSettings(QTUTILS_QT_NSP QSettings::Format a_format)
{
    Settings::setDefaultFormat(a_format);
}


QTUTILS_EXPORT void CleanupSettings(void)
{
    ::std::lock_guard< ::std::mutex> aGualrd(s_mutex);
    if(s_pSettings){
        const Qt::ConnectionType callCon = QThread::currentThread()==s_pSettings->thread()?Qt::AutoConnection:Qt::BlockingQueuedConnection;
        QMetaObject::invokeMethod(s_pSettings,[](){
            delete s_pSettings;
            s_pSettings = nullptr;
        },callCon);
    }
}


}  // namespace qtutils{


#else

namespace qtutils{

QTUTILS_EXPORT void InitializeSettings(QTUTILS_QT_NSP QSettings::Format a_format)
{
    QSettings::setDefaultFormat(a_format);
}


QTUTILS_EXPORT void CleanupSettings(void)
{
    //
}

}  // namespace qtutils{


#endif  // #if defined(CPPUTILS_POSSIBLE_NO_PERS_FILE) || defined(QTUTILS_CORE_FORCE_NEW_SETTINGS)

