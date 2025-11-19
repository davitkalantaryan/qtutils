//
// file:            qtutils_networkaccessmanager.cpp
// path:			src/core/qtutils_networkaccessmanager.cpp
// created on:		2021 Nov 27
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/core/networkaccessmanagerqu.hpp>
#include <qtutils/core/invokeblocked.hpp>
#include <qtutils/core/utils.hpp>
#include <new>
#include <string.h>
#include <stdlib.h>
#include <qtutils/disable_utils_warnings.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QThread>
#include <QCoreApplication>


namespace qtutils { namespace network {

#define QTUTILS_SEED_TABLE_SIZE     1024


Exception::Exception(network::ReplyData* a_pReplyData,const char* a_cpcWhat)
	:
	  ::std::runtime_error(a_cpcWhat),
	  m_pReplyData(a_pReplyData)
{
}

/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/


AccessManager::~AccessManager()
{
    m_bShouldRun = false;
    Reply *pReplyNext, *pReply;

    if(m_pQtManager){
        pReply=m_pFirstSimple;
        while(pReply){
            pReplyNext = pReply->m_itr.next;
            pReply->FromDestructorOfAccessManager();
            pReply->Abort();
            pReply->deleteLater();
            pReply = pReplyNext;
        }

        pReply=m_pFirstDependent;
        while(pReply){
            pReplyNext = pReply->m_itr.next;
            pReply->FromDestructorOfAccessManager();
            pReply->Abort();
            pReply->deleteLater();
            pReply = pReplyNext;
        }

        pReply=m_pFirstSingleton;
        while(pReply){
            pReplyNext = pReply->m_itr.next;
            pReply->FromDestructorOfAccessManager();
            pReply->Abort();
            pReply->deleteLater();
            pReply = pReplyNext;
        }

        m_pQtManager->deleteLater();
        m_pQtManager = nullptr;
    }
    else{
        pReply=m_pFirstWaiting;
        while(pReply){
            pReplyNext = pReply->m_itr.next;
            pReply->FromDestructorOfAccessManager();
            pReply->Abort();
            pReply->deleteLater();
            pReply = pReplyNext;
        }
    }

    free(m_seedData_p);
}


AccessManager::AccessManager()
    :
    m_seedData_p((SeedDataPtr*)calloc(QTUTILS_SEED_TABLE_SIZE,sizeof(SeedDataPtr))),
    m_destruct([](){})
{
    static int snIsNotInited = 1;
    if(snIsNotInited){
        qRegisterMetaType< QTUTILS_CORE_NTDT_NSP QtUtilsNetReplyArg >( "qtutils_QtUtilsNetReplyArg" );
        snIsNotInited = 0;
    }

    m_pQtManager = nullptr;
    m_pFirstWaiting = nullptr;
    m_pFirstSimple = m_pLastSimple = nullptr;
    m_pFirstSingleton = m_pLastSingleton = nullptr;
    m_pFirstDependent = m_pLastDependent = nullptr;
    m_bShouldRun = true;
    m_bPendingRestart = false;
    m_bQuitAppInDestuctor = false;

    if(!m_seedData_p){
        throw ::std::bad_alloc();
    }
}


void AccessManager::Restart()
{
    if(m_pQtManager){
        QNetworkAccessManager* pCurrent = m_pQtManager;
        m_pQtManager = nullptr;
        m_bPendingRestart = true;
        QObject::connect(pCurrent,&QObject::destroyed,pCurrent,[this](){
            m_bPendingRestart = false;
        });
        pCurrent->deleteLater();
    }
}


void AccessManager::QuitApp(const ::std::function<void(void)>& a_destruct)
{
    m_bShouldRun = false;
    m_bQuitAppInDestuctor = true;
    m_destruct = a_destruct;

    if(m_pQtManager){
        bool bAllRepliesDeleted = true;
        Reply *pReplyNext, *pReply;

        pReply=m_pFirstSimple;
        while(pReply){
            pReplyNext = pReply->m_itr.next;
            if(pReply->blockAppExit()){
                bAllRepliesDeleted = false;
            }
            else{
                pReply->Abort();
            }
            pReply = pReplyNext;
        }

        pReply=m_pFirstDependent;
        while(pReply){
            pReplyNext = pReply->m_itr.next;
            if(pReply->blockAppExit()){
                bAllRepliesDeleted = false;
            }
            else{
                pReply->Abort();
            }
            pReply = pReplyNext;
        }

        pReply=m_pFirstSingleton;
        while(pReply){
            pReplyNext = pReply->m_itr.next;
            if(pReply->blockAppExit()){
                bAllRepliesDeleted = false;
            }
            else{
                pReply->Abort();
            }
            pReply = pReplyNext;
        }

        if(bAllRepliesDeleted){
            a_destruct();
            QMetaObject::invokeMethod(qApp,[](){
                QCoreApplication::quit();
            });
        }
    }  //  if(m_pQtManager){
    else{
        a_destruct();
        QMetaObject::invokeMethod(qApp,[](){
            QCoreApplication::quit();
        });
    }
}


void AccessManager::RunReplyIfAllowedRaw(Reply* CPPUTILS_ARG_NN a_pReply)
{
    bool bReturn = true;
    a_pReply->m_bAdded = true;

    switch(a_pReply->m_callType){
    case CallType::SimpleDropSeed:{
        if(!SeedCheckAndAdd(a_pReply)){
            a_pReply->m_bAdded = false;
            a_pReply->deleteLater();
            return;
        }
        AddSimple(a_pReply);
    }break;
    case CallType::SimpleKeepSeed:{
        if(!SeedCheckAndAdd(a_pReply)){
            bReturn = false;
        }
        AddSimple(a_pReply);
    }break;
    case CallType::SingletonDropSeed:{
        if(SeedCheckAndAdd(a_pReply)){
            if(m_pFirstSingleton || m_pFirstDependent){
                bReturn = false;
            }
            AddSingleton(a_pReply);
        }
        else{
            a_pReply->m_bAdded = false;
            a_pReply->deleteLater();
            return;
        }
    }break;
    case CallType::SingletonKeepSeed:{
        if(SeedCheckAndAdd(a_pReply)){
            if(m_pFirstSingleton || m_pFirstDependent){
                bReturn = false;
            }
        }
        else{
            bReturn = false;
        }
        AddSingleton(a_pReply);
    }break;
    case CallType::DependentDropSeed:{
        if(SeedCheckAndAdd(a_pReply)){
            if(m_pFirstSingleton){
                bReturn = false;
            }
            AddDependent(a_pReply);
        }
        else{
            a_pReply->m_bAdded = false;
            a_pReply->deleteLater();
            return;
        }
    }break;
    case CallType::DependentKeepSeed:{
        if(SeedCheckAndAdd(a_pReply)){
            if(m_pFirstSingleton){
                bReturn = false;
            }
        }
        else{
            bReturn = false;
        }
        AddDependent(a_pReply);
    }break;
    default:
        AddSimple(a_pReply);
        break;
    }  //  switch(a_pReply->m_callType){

    if(bReturn){
        a_pReply->RunFunction();
    }
}


Reply* AccessManager::CreateAndAddReply(const CallType& a_callType, unsigned int a_seed, int a_timeoutMs, ReplyData* a_pData, const TypeLateActionFunc& a_func)
{
    Reply* const pReply = new Reply(a_callType,a_seed,a_timeoutMs,a_pData,a_func);

    if(!m_pQtManager){
        if(m_bPendingRestart){
            pReply->m_itr.prev = nullptr;
            pReply->m_itr.next = m_pFirstWaiting;
            if(m_pFirstWaiting){
                m_pFirstWaiting->m_itr.prev = pReply;
            }
            m_pFirstWaiting = pReply;
            pReply->m_bAdded = true;
            return pReply;
        }
        m_pQtManager = new QNetworkAccessManager();

        Reply *pReplyTmpNext, *pReplyTmp = m_pFirstWaiting;
        while(pReplyTmp){
            pReplyTmpNext = pReplyTmp->m_itr.next;
            pReplyTmp->m_itr.next = pReplyTmp->m_itr.prev = nullptr;
            RunReplyIfAllowedRaw(pReplyTmp);
            pReplyTmp = pReplyTmpNext;
        }
        m_pFirstWaiting = nullptr;

    }  //  if(!m_pQtManager){

    RunReplyIfAllowedRaw(pReply);

    ::QObject::connect(pReply, &Reply::finished, pReply,[this,pReply](QTUTILS_CORE_NTDT_NSP QtUtilsNetReplyArg){
        RemoveReply(pReply);
        if((!m_bShouldRun)&&(!m_pFirstSimple)&&(!m_pFirstSingleton)&&(!m_pFirstDependent)){
            if(m_pQtManager){
                QNetworkAccessManager* const pQtManager = m_pQtManager;
                m_pQtManager = nullptr;
                pQtManager->deleteLater();
            }
            if(m_bQuitAppInDestuctor){
                m_destruct();
                QMetaObject::invokeMethod(qApp,[this](){
                    QCoreApplication::quit();
                });
            }
        }
    });

    return pReply;
}


void AccessManager::RemoveReply(Reply* CPPUTILS_ARG_NN a_pReply)
{
    if(!(a_pReply->m_bAdded)){
        return;
    }

    a_pReply->m_bAdded = false;

    unsigned int nIndex;
    FindAndRemoveSeed(a_pReply->m_seed, a_pReply->m_callType);
    if(m_pQtManager){
        Reply **ppFirst, **ppLast;
        switch(a_pReply->m_callType){
        case CallType::SimpleDropSeed:case CallType::SimpleKeepSeed:
            ppFirst = &m_pFirstSimple;
            ppLast = &m_pLastSimple;
            break;
        case CallType::SingletonDropSeed: case CallType::SingletonKeepSeed:
            ppFirst = &m_pFirstSingleton;
            ppLast = &m_pLastSingleton;
            break;
        case CallType::DependentDropSeed:case CallType::DependentKeepSeed:
            ppFirst = &m_pFirstDependent;
            ppLast = &m_pLastDependent;
            break;
        default:
            ppFirst = &m_pFirstSimple;
            ppLast = &m_pLastSimple;
            break;
        }  //  switch(a_pReply->m_callType){

        if(a_pReply->m_itr.next){
            a_pReply->m_itr.next->m_itr.prev=a_pReply->m_itr.prev;
        }
        else{
            *ppLast = a_pReply->m_itr.prev;
        }

        if(a_pReply->m_itr.prev){
            a_pReply->m_itr.prev->m_itr.next=a_pReply->m_itr.next;
        }
        else{
            *ppFirst = a_pReply->m_itr.next;
        }
    }
    else{
        if(a_pReply->m_itr.next){
            a_pReply->m_itr.next->m_itr.prev=a_pReply->m_itr.prev;
        }

        if(a_pReply->m_itr.prev){
            a_pReply->m_itr.prev->m_itr.next=a_pReply->m_itr.next;
        }
        else{
            m_pFirstWaiting = a_pReply->m_itr.next;
        }

        return;
    }

    if(m_pFirstSingleton){
        Reply *pSingletonTmp, *pSingletonNext, *pSingleton = m_pFirstSingleton;
        while(pSingleton){
            pSingletonNext = pSingleton->m_itr.next;
            pSingletonTmp = findSeedRaw(pSingleton->m_seed,&nIndex);
            if((!pSingletonTmp)||(pSingletonTmp==pSingleton)){
                pSingleton->RunFunction();
                break;
            }
            pSingleton = pSingletonNext;
        }
    }
    else{
        Reply *pDependentTmp, *pDependentNext, *pDependent = m_pFirstDependent;
        while(pDependent){
            pDependentNext = pDependent->m_itr.next;
            pDependentTmp = findSeedRaw(pDependent->m_seed,&nIndex);
            if((!pDependentTmp)||(pDependentTmp==pDependent)){
                pDependent->RunFunction();
            }
            pDependent = pDependentNext;
        }
    }


    {
        Reply *pSimpleTmp, *pSimpleNext, *pSimple = m_pFirstSimple;
        while(pSimple){
            pSimpleNext = pSimple->m_itr.next;
            pSimpleTmp = findSeedRaw(pSimple->m_seed,&nIndex);
            if((!pSimpleTmp)||(pSimpleTmp==pSimple)){
                pSimple->RunFunction();
            }
            pSimple = pSimpleNext;
        }
    }
}


void AccessManager::AddSimple(Reply* CPPUTILS_ARG_NN a_pReply)
{
    a_pReply->m_itr.prev = m_pLastSimple;
    a_pReply->m_itr.next = nullptr;
    if(m_pLastSimple){
        m_pLastSimple->m_itr.next = a_pReply;
    }
    else{
        m_pFirstSimple = a_pReply;
    }
    m_pLastSimple = a_pReply;
}


void AccessManager::AddSingleton(Reply* CPPUTILS_ARG_NN a_pReply)
{
    a_pReply->m_itr.prev = m_pLastSingleton;
    a_pReply->m_itr.next = nullptr;
    if(m_pLastSingleton){
        m_pLastSingleton->m_itr.next = a_pReply;
    }
    else{
        m_pFirstSingleton = a_pReply;
    }
    m_pLastSingleton = a_pReply;
}


void AccessManager::AddDependent(Reply* CPPUTILS_ARG_NN a_pReply)
{
    a_pReply->m_itr.prev = m_pLastDependent;
    a_pReply->m_itr.next = nullptr;
    if(m_pLastDependent){
        m_pLastDependent->m_itr.next = a_pReply;
    }
    else{
        m_pFirstDependent = a_pReply;
    }
    m_pLastDependent = a_pReply;
}


Reply* AccessManager::findSeedRaw(unsigned int a_seed, unsigned int* CPPUTILS_ARG_NN a_nIndex_p)const
{
    *a_nIndex_p = a_seed % QTUTILS_SEED_TABLE_SIZE;
    Reply* pSeedData = m_seedData_p[*a_nIndex_p];
    while(pSeedData){
        if((pSeedData->m_seed)==a_seed){
            return pSeedData;
        }
        pSeedData = pSeedData->m_sed.next;
    }
    return nullptr;
}


bool AccessManager::SeedCheckAndAdd(Reply* CPPUTILS_ARG_NN a_pReply)
{
    if(a_pReply->m_seed!=QTUTILS_NET_NO_SEED){
        unsigned int nIndex;
        Reply* pSeedData = findSeedRaw(a_pReply->m_seed,&nIndex);
        if(pSeedData){
            return (pSeedData==a_pReply);
        }

        a_pReply->m_sed.prev = nullptr;
        a_pReply->m_sed.next = m_seedData_p[nIndex];
        if(m_seedData_p[nIndex]){
            m_seedData_p[nIndex]->m_sed.prev = a_pReply;
        }
        m_seedData_p[nIndex] = a_pReply;
    }

    return true;
}


void AccessManager::FindAndRemoveSeed(unsigned int a_seed, const CallType& a_type)
{
    switch(a_type){
    case CallType::None:
        break;
    default:{
        if(a_seed>0){
            unsigned int nIndex;
            Reply* pSeedData = findSeedRaw(a_seed,&nIndex);
            if(pSeedData){
                if(pSeedData->m_sed.next){pSeedData->m_sed.next->m_sed.prev=pSeedData->m_sed.prev;}
                if(pSeedData->m_sed.prev){
                    pSeedData->m_sed.prev->m_sed.next=pSeedData->m_sed.next;
                }
                else{
                    m_seedData_p[nIndex] = pSeedData->m_sed.next;
                }
                return;
            }  //  if(pSeedData){
        }  //  if(a_seed>0){
    }break;
    }  //  switch(a_type){
}


/*////   Simple   //////////*/

Reply* AccessManager::post(const QNetworkRequest& a_request, const QByteArray& a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    // const CallType& a_callType, unsigned int a_seed, int a_timeoutMs, ReplyData* a_pData, const TypeLateActionFunc& a_func
    Reply* const pReply = CreateAndAddReply(CallType::SimpleKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request,a_data](){
        return m_pQtManager->post(a_request,a_data);
    });
    return pReply;
}


Reply* AccessManager::post(const QNetworkRequest& a_request, QIODevice* a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(CallType::SimpleKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request,a_data](){
        return m_pQtManager->post(a_request,a_data);
    });
    return pReply;
}


Reply* AccessManager::post(const QNetworkRequest& a_request, const QVariantMap& a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    const QJsonDocument dataJsonDoc = QJsonDocument(QJsonObject::fromVariantMap(a_data));
    const QByteArray dataBA = dataJsonDoc.toJson(QJsonDocument::Compact);
    return this->post(a_request,dataBA,a_timeoutMs,a_pData,a_seed);
}


Reply* AccessManager::put(const QNetworkRequest& a_request, const QByteArray& a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(CallType::SimpleKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request,a_data](){
        return m_pQtManager->put(a_request,a_data);
    });
    return pReply;
}


Reply* AccessManager::put(const QNetworkRequest& a_request, QIODevice* a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(CallType::SimpleKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request,a_data](){
        return m_pQtManager->put(a_request,a_data);
    });
    return pReply;
}


Reply* AccessManager::put(const QNetworkRequest& a_request, const QVariantMap& a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    const QJsonDocument dataJsonDoc = QJsonDocument(QJsonObject::fromVariantMap(a_data));
    const QByteArray dataBA = dataJsonDoc.toJson(QJsonDocument::Compact);
    return this->put(a_request,dataBA,a_timeoutMs,a_pData,a_seed);
}


Reply* AccessManager::sendCustomRequest(const QNetworkRequest& a_request, const QByteArray& a_verb, const QByteArray& a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(CallType::SimpleKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request,a_verb,a_data](){
        return m_pQtManager->sendCustomRequest(a_request,a_verb,a_data);
    });
    return pReply;
}


Reply* AccessManager::sendCustomRequest(const QNetworkRequest& a_request, const QByteArray& a_verb, QIODevice* a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(CallType::SimpleKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request,a_verb,a_data](){
        return m_pQtManager->sendCustomRequest(a_request,a_verb,a_data);
    });
    return pReply;
}


Reply* AccessManager::sendCustomRequest(const QNetworkRequest& a_request, const QByteArray& a_verb, const QVariantMap& a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    const QJsonDocument dataJsonDoc = QJsonDocument(QJsonObject::fromVariantMap(a_data));
    const QByteArray dataBA = dataJsonDoc.toJson(QJsonDocument::Compact);
    return this->sendCustomRequest(a_request,a_verb,dataBA,a_timeoutMs,a_pData,a_seed);
}


Reply* AccessManager::get(const QNetworkRequest& a_request, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(CallType::SimpleKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request](){
        return m_pQtManager->get(a_request);
    });
    return pReply;
}


Reply* AccessManager::head(const QNetworkRequest& a_request, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(CallType::SimpleKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request](){
        return m_pQtManager->head(a_request);
    });
    return pReply;
}


Reply* AccessManager::deleteResource(const QNetworkRequest& a_request, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(CallType::SimpleKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request](){
        return m_pQtManager->deleteResource(a_request);
    });
    return pReply;
}


#ifdef QTUTILS_EXTRA_REST_CALLS

Reply* AccessManager::post(const QNetworkRequest& a_request, QHttpMultiPart* a_pMultiPart, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(CallType::SimpleKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request,a_pMultiPart](){
        return m_pQtManager->post(a_request,a_pMultiPart);
    });
    return pReply;
}


Reply* AccessManager::put(const QNetworkRequest &a_request, QHttpMultiPart* a_pMultiPart, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    //throw Exception(a_pData,"Unable to create Network Reply object");
    Reply* const pReply = CreateAndAddReply(CallType::SimpleKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request,a_pMultiPart](){
        return m_pQtManager->post(a_request,a_pMultiPart);
    });
    return pReply;
}


Reply* AccessManager::sendCustomRequest(const QNetworkRequest& a_request, const QByteArray& a_verb, QHttpMultiPart* a_pMultiPart, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(CallType::SimpleKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request,a_verb,a_pMultiPart](){
        return m_pQtManager->sendCustomRequest(a_request,a_verb,a_pMultiPart);
    });
    return pReply;
}

#endif  //  #ifdef QTUTILS_EXTRA_REST_CALLS


/*////   Singleton   //////////*/

Reply* AccessManager::postSingleton(const QNetworkRequest& a_request, const QByteArray& a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(CallType::SingletonKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request,a_data](){
        return m_pQtManager->post(a_request,a_data);
    });
    return pReply;
}


Reply* AccessManager::postSingleton(const QNetworkRequest& a_request, QIODevice* a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(CallType::SingletonKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request,a_data](){
        return m_pQtManager->post(a_request,a_data);
    });
    return pReply;
}


Reply* AccessManager::postSingleton(const QNetworkRequest& a_request, const QVariantMap& a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    const QJsonDocument dataJsonDoc = QJsonDocument(QJsonObject::fromVariantMap(a_data));
    const QByteArray dataBA = dataJsonDoc.toJson(QJsonDocument::Compact);
    return this->postSingleton(a_request,dataBA,a_timeoutMs,a_pData,a_seed);
}


Reply* AccessManager::putSingleton(const QNetworkRequest& a_request, const QByteArray& a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(CallType::SingletonKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request,a_data](){
        return m_pQtManager->put(a_request,a_data);
    });
    return pReply;
}


Reply* AccessManager::putSingleton(const QNetworkRequest& a_request, QIODevice* a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(CallType::SingletonKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request,a_data](){
        return m_pQtManager->put(a_request,a_data);
    });
    return pReply;
}


Reply* AccessManager::putSingleton(const QNetworkRequest& a_request, const QVariantMap& a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    const QJsonDocument dataJsonDoc = QJsonDocument(QJsonObject::fromVariantMap(a_data));
    const QByteArray dataBA = dataJsonDoc.toJson(QJsonDocument::Compact);
    return this->putSingleton(a_request,dataBA,a_timeoutMs,a_pData,a_seed);
}


Reply* AccessManager::sendCustomRequestSingleton(const QNetworkRequest& a_request, const QByteArray& a_verb, const QByteArray& a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(CallType::SingletonKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request,a_verb,a_data](){
        return m_pQtManager->sendCustomRequest(a_request,a_verb,a_data);
    });
    return pReply;
}


Reply* AccessManager::sendCustomRequestSingleton(const QNetworkRequest& a_request, const QByteArray& a_verb, QIODevice* a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(CallType::SingletonKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request,a_verb,a_data](){
        return m_pQtManager->sendCustomRequest(a_request,a_verb,a_data);
    });
    return pReply;
}


Reply* AccessManager::sendCustomRequestSingleton(const QNetworkRequest& a_request, const QByteArray& a_verb, const QVariantMap& a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    const QJsonDocument dataJsonDoc = QJsonDocument(QJsonObject::fromVariantMap(a_data));
    const QByteArray dataBA = dataJsonDoc.toJson(QJsonDocument::Compact);
    return this->sendCustomRequestSingleton(a_request,a_verb,dataBA,a_timeoutMs,a_pData,a_seed);
}


Reply* AccessManager::getSingleton(const QNetworkRequest& a_request, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(CallType::SingletonKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request](){
        return m_pQtManager->get(a_request);
    });
    return pReply;
}


Reply* AccessManager::headSingleton(const QNetworkRequest& a_request, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(CallType::SingletonKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request](){
        return m_pQtManager->head(a_request);
    });
    return pReply;
}


Reply* AccessManager::deleteResourceSingleton(const QNetworkRequest& a_request, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(CallType::SingletonKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request](){
        return m_pQtManager->deleteResource(a_request);
    });
    return pReply;
}


#ifdef QTUTILS_EXTRA_REST_CALLS

Reply* AccessManager::postSingleton(const QNetworkRequest& a_request, QHttpMultiPart* a_pMultiPart, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(CallType::SingletonKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request,a_pMultiPart](){
        return m_pQtManager->post(a_request,a_pMultiPart);
    });
    return pReply;
}


Reply* AccessManager::putSingleton(const QNetworkRequest &a_request, QHttpMultiPart* a_pMultiPart, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(CallType::SingletonKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request,a_pMultiPart](){
        return m_pQtManager->post(a_request,a_pMultiPart);
    });
    return pReply;
}


Reply* AccessManager::sendCustomRequestSingleton(const QNetworkRequest& a_request, const QByteArray& a_verb, QHttpMultiPart* a_pMultiPart, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(CallType::SingletonKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request,a_verb,a_pMultiPart](){
        return m_pQtManager->sendCustomRequest(a_request,a_verb,a_pMultiPart);
    });
    return pReply;
}

#endif  //  #ifdef QTUTILS_EXTRA_REST_CALLS


/*////   Dependent   //////////*/

Reply* AccessManager::postDependent(const QNetworkRequest& a_request, const QByteArray& a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(CallType::DependentKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request,a_data](){
        return m_pQtManager->post(a_request,a_data);
    });
    return pReply;
}


Reply* AccessManager::postDependent(const QNetworkRequest& a_request, QIODevice* a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(CallType::DependentKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request,a_data](){
        return m_pQtManager->post(a_request,a_data);
    });
    return pReply;
}


Reply* AccessManager::postDependent(const QNetworkRequest& a_request, const QVariantMap& a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    const QJsonDocument dataJsonDoc = QJsonDocument(QJsonObject::fromVariantMap(a_data));
    const QByteArray dataBA = dataJsonDoc.toJson(QJsonDocument::Compact);
    return this->postDependent(a_request,dataBA,a_timeoutMs,a_pData,a_seed);
}


Reply* AccessManager::putDependent(const QNetworkRequest& a_request, const QByteArray& a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(CallType::DependentKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request,a_data](){
        return m_pQtManager->put(a_request,a_data);
    });
    return pReply;
}


Reply* AccessManager::putDependent(const QNetworkRequest& a_request, QIODevice* a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(CallType::DependentKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request,a_data](){
        return m_pQtManager->put(a_request,a_data);
    });
    return pReply;
}


Reply* AccessManager::putDependent(const QNetworkRequest& a_request, const QVariantMap& a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    const QJsonDocument dataJsonDoc = QJsonDocument(QJsonObject::fromVariantMap(a_data));
    const QByteArray dataBA = dataJsonDoc.toJson(QJsonDocument::Compact);
    return this->putDependent(a_request,dataBA,a_timeoutMs,a_pData,a_seed);
}


Reply* AccessManager::sendCustomRequestDependent(const QNetworkRequest& a_request, const QByteArray& a_verb, const QByteArray& a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(CallType::DependentKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request,a_verb,a_data](){
        return m_pQtManager->sendCustomRequest(a_request,a_verb,a_data);
    });
    return pReply;
}


Reply* AccessManager::sendCustomRequestDependent(const QNetworkRequest& a_request, const QByteArray& a_verb, QIODevice* a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(CallType::DependentKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request,a_verb,a_data](){
        return m_pQtManager->sendCustomRequest(a_request,a_verb,a_data);
    });
    return pReply;
}


Reply* AccessManager::sendCustomRequestDependent(const QNetworkRequest& a_request, const QByteArray& a_verb, const QVariantMap& a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    const QJsonDocument dataJsonDoc = QJsonDocument(QJsonObject::fromVariantMap(a_data));
    const QByteArray dataBA = dataJsonDoc.toJson(QJsonDocument::Compact);
    return this->sendCustomRequestDependent(a_request,a_verb,dataBA,a_timeoutMs,a_pData,a_seed);
}


Reply* AccessManager::getDependent(const QNetworkRequest& a_request, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(CallType::DependentKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request](){
        return m_pQtManager->get(a_request);
    });
    return pReply;
}


Reply* AccessManager::headDependent(const QNetworkRequest& a_request, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(CallType::DependentKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request](){
        return m_pQtManager->head(a_request);
    });
    return pReply;
}


Reply* AccessManager::deleteResourceDependent(const QNetworkRequest& a_request, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(CallType::DependentKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request](){
        return m_pQtManager->deleteResource(a_request);
    });
    return pReply;
}


#ifdef QTUTILS_EXTRA_REST_CALLS

Reply* AccessManager::postDependent(const QNetworkRequest& a_request, QHttpMultiPart* a_pMultiPart, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(CallType::DependentKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request,a_pMultiPart](){
        return m_pQtManager->post(a_request,a_pMultiPart);
    });
    return pReply;
}


Reply* AccessManager::putDependent(const QNetworkRequest &a_request, QHttpMultiPart* a_pMultiPart, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    //throw Exception(a_pData,"Unable to create Network Reply object");
    Reply* const pReply = CreateAndAddReply(CallType::DependentKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request,a_pMultiPart](){
        return m_pQtManager->post(a_request,a_pMultiPart);
    });
    return pReply;
}


Reply* AccessManager::sendCustomRequestDependent(const QNetworkRequest& a_request, const QByteArray& a_verb, QHttpMultiPart* a_pMultiPart, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(CallType::DependentKeepSeed,a_seed,a_timeoutMs,a_pData,[this,a_request,a_verb,a_pMultiPart](){
        return m_pQtManager->sendCustomRequest(a_request,a_verb,a_pMultiPart);
    });
    return pReply;
}

#endif  //  #ifdef QTUTILS_EXTRA_REST_CALLS


/*////   Any   //////////*/

Reply* AccessManager::postAny(const CallType& a_callType, const QNetworkRequest& a_request, const QByteArray& a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(a_callType,a_seed,a_timeoutMs,a_pData,[this,a_request,a_data](){
        return m_pQtManager->post(a_request,a_data);
    });
    return pReply;
}


Reply* AccessManager::postAny(const CallType& a_callType, const QNetworkRequest& a_request, QIODevice* a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(a_callType,a_seed,a_timeoutMs,a_pData,[this,a_request,a_data](){
        return m_pQtManager->post(a_request,a_data);
    });
    return pReply;
}


Reply* AccessManager::postAny(const CallType& a_callType, const QNetworkRequest& a_request, const QVariantMap& a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    const QJsonDocument dataJsonDoc = QJsonDocument(QJsonObject::fromVariantMap(a_data));
    const QByteArray dataBA = dataJsonDoc.toJson(QJsonDocument::Compact);
    return this->postAny(a_callType,a_request,dataBA,a_timeoutMs,a_pData,a_seed);
}


Reply* AccessManager::putAny(const CallType& a_callType, const QNetworkRequest& a_request, const QByteArray& a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(a_callType,a_seed,a_timeoutMs,a_pData,[this,a_request,a_data](){
        return m_pQtManager->put(a_request,a_data);
    });
    return pReply;
}


Reply* AccessManager::putAny(const CallType& a_callType, const QNetworkRequest& a_request, QIODevice* a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(a_callType,a_seed,a_timeoutMs,a_pData,[this,a_request,a_data](){
        return m_pQtManager->put(a_request,a_data);
    });
    return pReply;
}


Reply* AccessManager::putAny(const CallType& a_callType, const QNetworkRequest& a_request, const QVariantMap& a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    const QJsonDocument dataJsonDoc = QJsonDocument(QJsonObject::fromVariantMap(a_data));
    const QByteArray dataBA = dataJsonDoc.toJson(QJsonDocument::Compact);
    return this->putAny(a_callType,a_request,dataBA,a_timeoutMs,a_pData,a_seed);
}


Reply* AccessManager::sendCustomRequestAny(const CallType& a_callType, const QNetworkRequest& a_request, const QByteArray& a_verb, const QByteArray& a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(a_callType,a_seed,a_timeoutMs,a_pData,[this,a_request,a_verb,a_data](){
        return m_pQtManager->sendCustomRequest(a_request,a_verb,a_data);
    });
    return pReply;
}


Reply* AccessManager::sendCustomRequestAny(const CallType& a_callType, const QNetworkRequest& a_request, const QByteArray& a_verb, QIODevice* a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(a_callType,a_seed,a_timeoutMs,a_pData,[this,a_request,a_verb,a_data](){
        return m_pQtManager->sendCustomRequest(a_request,a_verb,a_data);
    });
    return pReply;
}


Reply* AccessManager::sendCustomRequestAny(const CallType& a_callType, const QNetworkRequest& a_request, const QByteArray& a_verb, const QVariantMap& a_data, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    const QJsonDocument dataJsonDoc = QJsonDocument(QJsonObject::fromVariantMap(a_data));
    const QByteArray dataBA = dataJsonDoc.toJson(QJsonDocument::Compact);
    return this->sendCustomRequestAny(a_callType,a_request,a_verb,dataBA,a_timeoutMs,a_pData,a_seed);
}


Reply* AccessManager::getAny(const CallType& a_callType, const QNetworkRequest& a_request, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(a_callType,a_seed,a_timeoutMs,a_pData,[this,a_request](){
        return m_pQtManager->get(a_request);
    });
    return pReply;
}


Reply* AccessManager::headAny(const CallType& a_callType, const QNetworkRequest& a_request, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(a_callType,a_seed,a_timeoutMs,a_pData,[this,a_request](){
        return m_pQtManager->head(a_request);
    });
    return pReply;
}


Reply* AccessManager::deleteResourceAny(const CallType& a_callType, const QNetworkRequest& a_request, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(a_callType,a_seed,a_timeoutMs,a_pData,[this,a_request](){
        return m_pQtManager->deleteResource(a_request);
    });
    return pReply;
}


#ifdef QTUTILS_EXTRA_REST_CALLS

Reply* AccessManager::postAny(const CallType& a_callType, const QNetworkRequest& a_request, QHttpMultiPart* a_pMultiPart, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(a_callType,a_seed,a_timeoutMs,a_pData,[this,a_request,a_pMultiPart](){
        return m_pQtManager->post(a_request,a_pMultiPart);
    });
    return pReply;
}


Reply* AccessManager::putAny(const CallType& a_callType, const QNetworkRequest &a_request, QHttpMultiPart* a_pMultiPart, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    //throw Exception(a_pData,"Unable to create Network Reply object");
    Reply* const pReply = CreateAndAddReply(a_callType,a_seed,a_timeoutMs,a_pData,[this,a_request,a_pMultiPart](){
        return m_pQtManager->post(a_request,a_pMultiPart);
    });
    return pReply;
}


Reply* AccessManager::sendCustomRequestAny(const CallType& a_callType, const QNetworkRequest& a_request, const QByteArray& a_verb, QHttpMultiPart* a_pMultiPart, int a_timeoutMs, ReplyData* a_pData, unsigned int a_seed)
{
    Reply* const pReply = CreateAndAddReply(a_callType,a_seed,a_timeoutMs,a_pData,[this,a_request,a_verb,a_pMultiPart](){
        return m_pQtManager->sendCustomRequest(a_request,a_verb,a_pMultiPart);
    });
    return pReply;
}

#endif  //  #ifdef QTUTILS_EXTRA_REST_CALLS



/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

ReplyData::~ReplyData()
{
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

Reply::~Reply()
{
    m_bFinishedEmited = true;

    Abort();

    delete m_pData;

    if(m_pNetworkReply){
        m_pNetworkReply->deleteLater();
    }
}


Reply::Reply(const CallType& a_callType, unsigned int a_seed, int a_timeoutMs, ReplyData* a_pData, const TypeLateActionFunc& a_func)
    :
    m_callType(a_callType),
    m_seed(a_seed),
    m_timeout(a_timeoutMs),
    m_func(a_func),
    m_pNetworkReply(nullptr),
    m_pData(a_pData)
{
    m_itr.next = m_itr.prev = m_sed.next = m_sed.prev = nullptr;
    m_bHasTimeout = false;
    m_bBlockAppExit = false;
    m_bFinishedEmited = false;
    m_bAdded = false;
}


bool Reply::blockAppExit()const
{
    return m_bBlockAppExit;
}


void Reply::SetBlockAppExit(bool a_bBlockAppExit)
{
    m_bBlockAppExit = a_bBlockAppExit;
}


void Reply::FromDestructorOfAccessManager()
{
    m_bAdded = false;
    m_bFinishedEmited = true;
}


void Reply::RunFunction()
{
    if(m_pNetworkReply){
        return;
    }

    m_pNetworkReply = m_func();
    if(!m_pNetworkReply){
        throw Exception(m_pData,"Unable to create Network Reply object");
    }

    m_connDestroy = connect(m_pNetworkReply,&QObject::destroyed,this,[this](){
        m_pNetworkReply = nullptr;
        deleteLater();
    });

    m_connFinished = ::QObject::connect(m_pNetworkReply,&QNetworkReply::finished,this,[this](){
        if(m_timeoutTimer.isActive()){
            const QMetaObject::Connection connTimeout = m_connTimeout;
            m_connTimeout = QMetaObject::Connection();
            ::QObject::disconnect(connTimeout);
            m_timeoutTimer.stop();
        }
        const QMetaObject::Connection connFinished = m_connFinished;
        m_connFinished = QMetaObject::Connection();
        ::QObject::disconnect(connFinished);
        if((!m_bFinishedEmited) && m_pNetworkReply){
            m_bFinishedEmited = true;
            emit finished(QTUTILS_CORE_NTDT_NSP QtUtilsNetReplyArg(this,[](::qtutils::network::Reply* a_pTs){a_pTs->deleteLater();}));
        }
    });

    if(m_timeout>=0){
        m_connTimeout = QObject::connect(&(m_timeoutTimer),&QTimer::timeout,this,[this](){
            m_bHasTimeout = true;

            const QMetaObject::Connection connTimeout = m_connTimeout;
            m_connTimeout = QMetaObject::Connection();
            ::QObject::disconnect(connTimeout);

            const QMetaObject::Connection connFinished = m_connFinished;
            m_connFinished = QMetaObject::Connection();
            ::QObject::disconnect(connFinished);

            Abort();
            if((!m_bFinishedEmited) && m_pNetworkReply){
                m_bFinishedEmited = true;
                emit finished(QTUTILS_CORE_NTDT_NSP QtUtilsNetReplyArg(this,[](::qtutils::network::Reply* a_pTs){a_pTs->deleteLater();}));
            }
        });
        m_timeoutTimer.start(m_timeout);
    }  //  if(m_timeout>=0){
}


QNetworkReply* Reply::operator->()const
{
    return m_pNetworkReply;
}


QNetworkReply* Reply::qtNetworkReply()const
{
    return m_pNetworkReply;
}


bool Reply::hasTimeout()const
{
    return m_bHasTimeout;
}


void Reply::Abort()
{
    if(m_timeoutTimer.isActive()){
        const QMetaObject::Connection connTimeout = m_connTimeout;
        m_connTimeout = QMetaObject::Connection();
        ::QObject::disconnect(connTimeout);
        m_timeoutTimer.stop();
    }

    if(m_connFinished){
        const QMetaObject::Connection connFinished = m_connFinished;
        m_connFinished = QMetaObject::Connection();
        ::QObject::disconnect(connFinished);
        if(m_pNetworkReply){m_pNetworkReply->abort();}
    }    
}


ReplyData* Reply::data()const
{
    return m_pData;
}


void Reply::ReplaceData(ReplyData* a_pData)
{
    m_pData = a_pData;
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

QTUTILS_EXPORT void PrepareHeadersRaw(const QByteArray& a_contTypeHeader, QNetworkRequest* a_pRequet, const QString& a_agent)
{
    a_pRequet->setHeader(QNetworkRequest::ContentTypeHeader, a_contTypeHeader);
    a_pRequet->setRawHeader("Client-Device", QSysInfo::machineHostName().toUtf8() );

#ifdef CPPUTILS_EMSCRIPTEN_IS_USED
    static_cast<void>(a_agent);  // each browser has its own agent
#else
    a_pRequet->setHeader(QNetworkRequest::UserAgentHeader, a_agent);  // each browser has its own agent

    {
        // todo: comment below 3 lines
        QSslConfiguration conf = a_pRequet->sslConfiguration();
        conf.setPeerVerifyMode(QSslSocket::VerifyNone);
        a_pRequet->setSslConfiguration(conf);
    }
    
#endif
}


QTUTILS_EXPORT void PrepareJsonHeaders(QNetworkRequest* a_pRequet, const QString& a_agent)
{
    PrepareHeadersRaw("application/json",a_pRequet,a_agent);
}


QTUTILS_EXPORT void PrepareJsonHeadersWithAuth(QNetworkRequest* a_pRequet, const QString& a_authToken,const QString& a_agent)
{
    ::std::string authString = ::std::string("Bearer ") + a_authToken.toStdString();
    a_pRequet->setRawHeader("Authorization",authString.c_str());
    PrepareJsonHeaders(a_pRequet, a_agent);
}


QTUTILS_EXPORT void PrepareMPartHeadersWithAuth(QNetworkRequest* a_pRequet, const QString& a_authToken,const QString& a_agent)
{
    ::std::string authString = ::std::string("Bearer ") + a_authToken.toStdString();
    a_pRequet->setRawHeader("Authorization",authString.c_str());
    PrepareMPartHeaders(a_pRequet,a_agent);
}


QTUTILS_EXPORT void PrepareMPartHeaders(QNetworkRequest* a_pRequet, const QString& a_agent)
{
    a_pRequet->setRawHeader("Client-Device", QSysInfo::machineHostName().toUtf8() );

#ifdef CPPUTILS_EMSCRIPTEN_IS_USED
    static_cast<void>(a_agent);  // each browser has its own agent
#else
    a_pRequet->setHeader(QNetworkRequest::UserAgentHeader, a_agent);  // each browser has its own agent

    {
        // todo: comment below 3 lines
        QSslConfiguration conf = a_pRequet->sslConfiguration();
        conf.setPeerVerifyMode(QSslSocket::VerifyNone);
        a_pRequet->setSslConfiguration(conf);
    }
    
#endif

    a_pRequet->setRawHeader("Accept", "*/*");
}


QTUTILS_EXPORT void ErrorByteArray(const QNetworkReply::NetworkError&,const ::qtutils::network::Reply& a_replyHandlerIn, QByteArray* CPPUTILS_IN_OUT a_pData)
{
    QByteArray& responseByteArray = *a_pData;
    if(responseByteArray.isEmpty()){
        if(a_replyHandlerIn.hasTimeout()){
            responseByteArray = "timeout";
        }
        else{
            responseByteArray = a_replyHandlerIn->errorString().toLocal8Bit();
            if(responseByteArray.isEmpty()){
                responseByteArray = "unknown";
            }
        }
    } // if(responseByteArray.isEmpty()){
}


QTUTILS_EXPORT QString CorectUrl(const QString& a_url)
{
	if((a_url.size()>0)&&(a_url.back()==QChar('/'))){
		QString retStr(a_url);
		retStr.chop(1);
		return retStr;
	}
	
	return a_url;
}


QTUTILS_EXPORT QString NetworkErrorCodeString(const QNetworkReply::NetworkError& a_errorCode)
{
    QString errStr = utils::QtEnumToString(a_errorCode);
    errStr.push_back('(');
    errStr += QString::number(static_cast<int>(a_errorCode));
    errStr.push_back(')');
    return errStr;
}


#ifdef QTUTILS_HTTP_SERVER_TOOLS
QTUTILS_EXPORT QByteArray HttpRequestMethodToByteArray(const QHttpServerRequest::Method& a_method)
{
    const QMetaEnum metaEnum = QMetaEnum::fromType<QHttpServerRequest::Method>();
    const char* cpcVerb =  metaEnum.valueToKey(static_cast<int>(a_method));
    if(!cpcVerb){
        return "";
    }
    const char* cpcLastDblPoints = strrchr(cpcVerb,':');
    if(cpcLastDblPoints){
        return QByteArray(cpcLastDblPoints+1).toUpper();
    }
    return QByteArray(cpcVerb).toUpper();
}
#endif


}}  // namespace qtutils { namespace network {
