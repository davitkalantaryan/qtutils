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


namespace qtutils { namespace network {

#define QTUTILS_SEED_TABLE_SIZE     1024


Exception::Exception(network::ReplyData* a_pReplyData,const char* a_cpcWhat)
	:
	  ::std::runtime_error(a_cpcWhat),
	  m_pReplyData(a_pReplyData)
{
}

/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/


AccessManagerRaw::~AccessManagerRaw()
{
    delete m_pQtManager;
    free(m_seedData_p);
}


AccessManagerRaw::AccessManagerRaw()
    :
    m_pQtManager(new QNetworkAccessManager()),
    m_seedData_p((SeedData**)calloc(QTUTILS_SEED_TABLE_SIZE,sizeof(SeedData*)))
{
    m_countOfCorelated = 0;
    if(!m_seedData_p){
        throw ::std::bad_alloc();
    }
}


Reply* AccessManagerRaw::CreateAndAddReply(QNetworkReply* CPPUTILS_ARG_NN a_pNetworkReply, int a_timeoutMs, ReplyData* a_pData, int a_seed)
{
    Reply* const pReturnReply = new Reply(a_pNetworkReply,a_pData, a_timeoutMs);

    pReturnReply->m_connFinished = ::QObject::connect(pReturnReply->m_pNetworkReply,&QNetworkReply::finished,pReturnReply,[this,pReturnReply](){
        if(pReturnReply->m_timeoutTimer.isActive()){
            const QMetaObject::Connection connTimeout = pReturnReply->m_connTimeout;
            pReturnReply->m_connTimeout = QMetaObject::Connection();
            ::QObject::disconnect(connTimeout);
            pReturnReply->m_timeoutTimer.stop();
        }
        const QMetaObject::Connection connFinished = pReturnReply->m_connFinished;
        pReturnReply->m_connFinished = QMetaObject::Connection();
        ::QObject::disconnect(connFinished);
        emit pReturnReply->finished(QTUTILS_CORE_NTDT_NSP QtUtilsNetReplyArg(this,[](::qtutils::network::Reply* a_pTs){a_pTs->deleteLater();}));
        RemoveReply(pReturnReply);
        pReturnReply->deleteLater();
    });

    if(a_timeoutMs>=0){
        pReturnReply->m_connTimeout = QObject::connect(&(pReturnReply->m_timeoutTimer),&QTimer::timeout,pReturnReply,[this](){
            pReturnReply->m_bHasTimeout = true;

            const QMetaObject::Connection connTimeout = pReturnReply->m_connTimeout;
            pReturnReply->m_connTimeout = QMetaObject::Connection();
            ::QObject::disconnect(connTimeout);

            const QMetaObject::Connection connFinished = pReturnReply->m_connFinished;
            pReturnReply->m_connFinished = QMetaObject::Connection();
            ::QObject::disconnect(connFinished);

            pReturnReply->Abort();
            emit pReturnReply->finished(QtUtilsNetReplyArg(this,[](::qtutils::network::Reply* a_pTs){a_pTs->deleteLater();}));
        });
        pReturnReply->m_timeoutTimer.start(a_timeoutMs);
    }


    pReturnReply->m_prev = nullptr;
    pReturnReply->m_next = m_pFirst;
    if(m_pFirst){
        m_pFirst->m_prev = pReturnReply;
    }
    m_pFirst = pReturnReply;
}


void AccessManagerRaw::RemoveDependentCallRaw()
{
    --m_countOfCorelated;
    if(m_pFirstSingleton){
        LateAction* const pFirstSingleton = m_pFirstSingleton;
        if(pFirstSingleton->next){
            m_pFirstSingleton = pFirstSingleton->next;
        }
        else{
            m_pFirstSingleton = m_pLastSingleton = nullptr;
        }
        (pFirstSingleton->func)();
    }
    else{
        LateAction *pDependentNext, *pDependent = m_pFirstDependent;
        while(pDependent){
            pDependentNext = pDependent->next;
            if(SeedCheckAndAdd(pDependent->seed)){
                (pDependent->func)();
            }
            pDependent = pDependentNext;
        }

        m_pFirstDependent = m_pLastDependent = nullptr;
    }
}


void AccessManagerRaw::RemoveReply(Reply* CPPUTILS_ARG_NN a_pReply)
{
    FindAndRemoveSeed(a_pReply->m_seed, a_pReply->m_callType);
    switch(a_pReply->m_callType){
    case CallType::DependentDropSeed:case CallType::DependentKeepSeed: case CallType::Singleton:
        RemoveDependentCallRaw();
        break;
    default:
        break;
    }  //  switch(a_pReply->m_callType){

    if(a_pReply->m_next){
        a_pReply->m_next->m_prev=a_pReply->m_prev;
    }

    if(a_pReply->m_prev){
        a_pReply->m_prev->m_next=a_pReply->m_next;
    }
    else{
        m_pFirst = a_pReply->m_next;
    }
}


void AccessManagerRaw::AddSingleton(const TypeLateActionFunc& a_func)
{
    LateAction* const pLateAction = new LateAction();
    pLateAction->prev = m_pLastSingleton;
    pLateAction->next = nullptr;
    if(m_pLastSingleton){
        m_pLastSingleton->prev = pLateAction;
    }
    else{
        m_pFirstSingleton = pLateAction;
    }
    m_pLastSingleton = pLateAction;
}


void AccessManagerRaw::AddDependent(const TypeLateActionFunc& a_func)
{
    LateAction* const pLateAction = new LateAction();
    pLateAction->prev = m_pLastDependent;
    pLateAction->next = nullptr;
    if(m_pLastDependent){
        m_pLastDependent->prev = pLateAction;
    }
    else{
        m_pFirstDependent = pLateAction;
    }
    m_pLastDependent = pLateAction;
}


SeedData* AccessManagerRaw::findSeedRaw(int a_seed, int* CPPUTILS_ARG_NN a_nIndex_p)const
{
    *a_nIndex_p = a_seed % QTUTILS_SEED_TABLE_SIZE;
    SeedData* pSeedData = m_seedData_p[*a_nIndex_p];
    while(pSeedData){
        if((pSeedData->seed)==a_seed){
            return pSeedData;
        }
        pSeedData = pSeedData->next;
    }
    return nullptr;
}


bool AccessManagerRaw::SeedCheckAndAdd(int a_seed)
{
    if(a_seed>0){
        int nIndex;
        SeedData* pSeedData = findSeedRaw(a_seed,&nIndex);
        if(pSeedData){
            return false;
        }

        pSeedData = new SeedData();
        pSeedData->prev = nullptr;
        pSeedData->next = m_seedData_p[nIndex];
        if(m_seedData_p[nIndex]){
            m_seedData_p[nIndex]->prev = pSeedData;
        }
        m_seedData_p[nIndex] = pSeedData;
    }

    return true;
}


void AccessManagerRaw::FindAndRemoveSeed(int a_seed, const CallType& a_type)
{
    switch(a_type){
    case CallType::Singleton:
        break;
    default:{
        if(a_seed>0){
            int nIndex;
            SeedData* pSeedData = findSeedRaw(a_seed,&nIndex);
            if(pSeedData){
                if(pSeedData->next){pSeedData->next->prev=pSeedData->prev;}
                if(pSeedData->prev){
                    pSeedData->prev->next=pSeedData->next;
                }
                else{
                    m_seedData_p[nIndex] = pSeedData->next;
                }
                return;
            }  //  if(pSeedData){
        }  //  if(a_seed>0){
    }break;
    }  //  switch(a_type){
}


/*////   Raw   //////////*/

Reply* AccessManagerRaw::postRaw(const QNetworkRequest& a_request, const QByteArray& a_data,  int a_timeoutMs, ReplyData* a_pData, int a_seed, const CallType& a_callType)
{
    QNetworkReply* pNetworkReply = m_pQtManager->post(a_request,a_data);
    if(pNetworkReply){
        return new Reply(a_callType,a_seed,pNetworkReply,a_pData, a_timeoutMs);
    }
	throw Exception(a_pData,"Unable to create Network Reply object");
}


Reply* AccessManagerRaw::postRaw(const QNetworkRequest& a_request, QIODevice* a_data, int a_timeoutMs, ReplyData* a_pData, int a_seed, const CallType& a_callType)
{
    QNetworkReply* pNetworkReply = m_pQtManager->post(a_request,a_data);
    if(pNetworkReply){
        return new Reply(a_callType,a_seed,pNetworkReply,a_pData, a_timeoutMs);
    }
	throw Exception(a_pData,"Unable to create Network Reply object");
}


Reply* AccessManagerRaw::postRaw(const QNetworkRequest& a_request, const QVariantMap& a_data,  int a_timeoutMs, ReplyData* a_pData, int a_seed, const CallType& a_callType)
{
    const QJsonDocument dataJsonDoc = QJsonDocument(QJsonObject::fromVariantMap(a_data));
    const QByteArray dataBA = dataJsonDoc.toJson(QJsonDocument::Compact);
    return postRaw(a_request,dataBA,a_pData,a_timeoutMs,a_seed,a_callType);
}


Reply* AccessManagerRaw::postRaw(const QNetworkRequest& a_request, QHttpMultiPart* a_pMultiPart,  int a_timeoutMs, ReplyData* a_pData, int a_seed, const CallType& a_callType)
{
    QNetworkReply* pNetworkReply = m_pQtManager->post(a_request,a_pMultiPart);
    if(pNetworkReply){
        return new Reply(a_callType,a_seed,pNetworkReply,a_pData, a_timeoutMs);
    }
    throw Exception(a_pData,"Unable to create Network Reply object");
}


Reply* AccessManagerRaw::putRaw(const QNetworkRequest& a_request, QIODevice* a_data, int a_timeoutMs, ReplyData* a_pData, int a_seed, const CallType& a_callType)
{
    QNetworkReply* pNetworkReply = m_pQtManager->put(a_request,a_data);
    if(pNetworkReply){
        return new Reply(a_callType,a_seed,pNetworkReply,a_pData, a_timeoutMs);
    }
	throw Exception(a_pData,"Unable to create Network Reply object");
}


Reply* AccessManagerRaw::putRaw(const QNetworkRequest& a_request, const QByteArray& a_data,  int a_timeoutMs, ReplyData* a_pData, int a_seed, const CallType& a_callType)
{
    QNetworkReply* pNetworkReply = m_pQtManager->put(a_request,a_data);
    if(pNetworkReply){
        return new Reply(a_callType,a_seed,pNetworkReply,a_pData, a_timeoutMs);
    }
	throw Exception(a_pData,"Unable to create Network Reply object");
}


Reply* AccessManagerRaw::putRaw(const QNetworkRequest& a_request, const QVariantMap& a_data,  int a_timeoutMs, ReplyData* a_pData, int a_seed, const CallType& a_callType)
{
    const QJsonDocument dataJsonDoc = QJsonDocument(QJsonObject::fromVariantMap(a_data));
    const QByteArray dataBA = dataJsonDoc.toJson(QJsonDocument::Compact);
    return putRaw(a_callType,a_request,dataBA,a_pData,a_timeoutMs);
}


Reply* AccessManagerRaw::sendCustomRequestRaw(const QNetworkRequest& a_request, const QByteArray& a_verb, const QVariantMap& a_data, int a_timeoutMs, ReplyData* a_pData, int a_seed, const CallType& a_callType)
{
    const QJsonDocument dataJsonDoc = QJsonDocument(QJsonObject::fromVariantMap(a_data));
    const QByteArray dataBA = dataJsonDoc.toJson(QJsonDocument::Compact);
    return sendCustomRequestRaw(a_callType,a_request,a_verb, dataBA,a_pData,a_timeoutMs,a_seed);
}


Reply* AccessManagerRaw::getRaw(const QNetworkRequest& a_request, int a_timeoutMs, ReplyData* a_pData, int a_seed, const CallType& a_callType)
{
    QNetworkReply* pNetworkReply = m_pQtManager->get(a_request);
    if(pNetworkReply){
        return new Reply(a_callType,a_seed,pNetworkReply,a_pData, a_timeoutMs);
    }
	throw Exception(a_pData,"Unable to create Network Reply object");
}


Reply* AccessManagerRaw::headRaw(const QNetworkRequest& a_request, int a_timeoutMs, ReplyData* a_pData, int a_seed, const CallType& a_callType)
{
    QNetworkReply* pNetworkReply = m_pQtManager->head(a_request);
    if(pNetworkReply){
        return new Reply(a_callType,a_seed,pNetworkReply,a_pData, a_timeoutMs);
    }
    throw Exception(a_pData,"Unable to create Network Reply object");
}


Reply* AccessManagerRaw::sendCustomRequestRaw(const QNetworkRequest& a_request, const QByteArray& a_verb, int a_timeoutMs, ReplyData* a_pData, int a_seed, QIODevice* a_data, const CallType& a_callType)
{
    QNetworkReply* pNetworkReply = m_pQtManager->sendCustomRequest(a_request,a_verb,a_data);
    if(pNetworkReply){
        return new Reply(a_callType,a_seed,pNetworkReply,a_pData, a_timeoutMs);
    }
	throw Exception(a_pData,"Unable to create Network Reply object");
}


Reply* AccessManagerRaw::sendCustomRequestRaw(const QNetworkRequest& a_request, const QByteArray& a_verb, const QByteArray& a_data, int a_timeoutMs, ReplyData* a_pData, int a_seed, const CallType& a_callType)
{
    QNetworkReply* pNetworkReply = m_pQtManager->sendCustomRequest(a_request,a_verb,a_data);
    if(pNetworkReply){
        return new Reply(a_callType,a_seed,pNetworkReply,a_pData, a_timeoutMs);
    }
	throw Exception(a_pData,"Unable to create Network Reply object");
}


Reply* AccessManagerRaw::deleteResourceRaw(const QNetworkRequest& a_request, int a_timeoutMs, ReplyData* a_pData, int a_seed, const CallType& a_callType)
{
    QNetworkReply* pNetworkReply = m_pQtManager->deleteResource(a_request);
    if(pNetworkReply){
        return new Reply(a_callType,a_seed,pNetworkReply,a_pData, a_timeoutMs);
    }
	throw Exception(a_pData,"Unable to create Network Reply object");
}


#ifdef QTUTILS_EXTRA_REST_CALLS

Reply* AccessManagerRaw::postRaw(const QNetworkRequest &a_request, QHttpMultiPart* a_multiPart, int a_timeoutMs, ReplyData* a_pData, int a_seed, const CallType& a_callType)
{
    QNetworkReply* pNetworkReply = m_pQtManager->post(a_request,a_multiPart);
    if(pNetworkReply){
        return new Reply(a_callType,a_seed,pNetworkReply,a_pData, a_timeoutMs);
    }
    throw Exception(a_pData,"Unable to create Network Reply object");
}


Reply* AccessManagerRaw::putRaw(const QNetworkRequest &a_request, QHttpMultiPart* a_multiPart, int a_timeoutMs, ReplyData* a_pData, int a_seed, const CallType& a_callType)
{
    QNetworkReply* pNetworkReply = m_pQtManager->put(a_request,a_multiPart);
    if(pNetworkReply){
        return new Reply(a_callType,a_seed,pNetworkReply,a_pData, a_timeoutMs);
    }
    throw Exception(a_pData,"Unable to create Network Reply object");
}


Reply* AccessManagerRaw::sendCustomRequestRaw(const QNetworkRequest& a_request, const QByteArray& a_verb, QHttpMultiPart* a_multiPart, int a_timeoutMs, ReplyData* a_pData, int a_seed, const CallType& a_callType)
{
    QNetworkReply* pNetworkReply = m_pQtManager->sendCustomRequest(a_request,a_verb,a_multiPart);
    if(pNetworkReply){
        return new Reply(a_callType,a_seed,pNetworkReply,a_pData, a_timeoutMs);
    }
    throw Exception(a_pData,"Unable to create Network Reply object");
}

#endif  //  #ifdef QTUTILS_EXTRA_REST_CALLS



/*////   Simple   //////////*/

Reply* AccessManagerRaw::post(const QNetworkRequest& a_request, const QByteArray& a_data, int a_timeoutMs, ReplyData* a_pData, int a_seed)
{
    return postRaw(a_request,a_data,a_pData,a_timeoutMs,CallType::Simple);
}


Reply* AccessManagerRaw::post(const QNetworkRequest& a_request, QIODevice* a_data, int a_timeoutMs, ReplyData* a_pData, int a_seed)
{
    return postRaw(a_request,a_data,a_pData,a_timeoutMs,CallType::Simple);
}


Reply* AccessManagerRaw::post(const QNetworkRequest& a_request, const QVariantMap& a_data, int a_timeoutMs, ReplyData* a_pData, int a_seed)
{
    return postRaw(a_request,a_data,a_pData,a_timeoutMs,CallType::Simple);
}


Reply* AccessManagerRaw::post(const QNetworkRequest& a_request, QHttpMultiPart* a_pMultiPart, int a_timeoutMs, ReplyData* a_pData, int a_seed)
{
    return postRaw(a_request,a_pMultiPart,a_pData,a_timeoutMs,CallType::Simple);
}


Reply* AccessManagerRaw::put(const QNetworkRequest& a_request, QIODevice* a_data, int a_timeoutMs, ReplyData* a_pData, int a_seed)
{
    return putRaw(a_request,a_data,a_pData,a_timeoutMs,CallType::Simple);
}


Reply* AccessManagerRaw::put(const QNetworkRequest& a_request, const QByteArray& a_data, int a_timeoutMs, ReplyData* a_pData, int a_seed)
{
    return putRaw(a_request,a_data,a_pData,a_timeoutMs,CallType::Simple);
}


Reply* AccessManagerRaw::put(const QNetworkRequest& a_request, const QVariantMap& a_data, int a_timeoutMs, ReplyData* a_pData, int a_seed)
{
    return putRaw(a_request,a_data,a_pData,a_timeoutMs,CallType::Simple);
}


Reply* AccessManagerRaw::sendCustomRequest(const QNetworkRequest& a_request, const QByteArray& a_verb, const QVariantMap& a_data, int a_timeoutMs, ReplyData* a_pData, int a_seed)
{
    return sendCustomRequestRaw(a_request,a_verb,a_data,a_pData,a_timeoutMs,CallType::Simple);
}


Reply* AccessManagerRaw::get(const QNetworkRequest& a_request, int a_timeoutMs, ReplyData* a_pData, int a_seed)
{
    return getRaw(a_request,a_pData,a_timeoutMs,CallType::Simple);
}


Reply* AccessManagerRaw::head(const QNetworkRequest& a_request, int a_timeoutMs, ReplyData* a_pData, int a_seed)
{
    return headRaw(a_request,a_pData,a_timeoutMs,CallType::Simple);
}


Reply* AccessManagerRaw::sendCustomRequest(const QNetworkRequest& a_request, const QByteArray& a_verb, QIODevice* a_data, int a_timeoutMs, ReplyData* a_pData, int a_seed)
{
    return sendCustomRequestRaw(a_request,a_verb,a_pData,a_timeoutMs,a_data,CallType::Simple);
}


Reply* AccessManagerRaw::sendCustomRequest(const QNetworkRequest& a_request, const QByteArray& a_verb, const QByteArray& a_data, int a_timeoutMs, ReplyData* a_pData, int a_seed)
{
    return sendCustomRequestRaw(a_request,a_verb,a_pData,a_timeoutMs,a_data,CallType::Simple);
}


Reply* AccessManagerRaw::deleteResource(const QNetworkRequest& a_request, int a_timeoutMs, ReplyData* a_pData, int a_seed)
{
    return deleteResourceRaw(a_request,a_pData,a_timeoutMs,CallType::Simple);
}


#ifdef QTUTILS_EXTRA_REST_CALLS

Reply* AccessManagerRaw::postRaw(const QNetworkRequest &a_request, QHttpMultiPart* a_multiPart, int a_timeoutMs, ReplyData* a_pData, int a_seed, const CallType& a_callType)
{
    QNetworkReply* pNetworkReply = m_pQtManager->post(a_request,a_multiPart);
    if(pNetworkReply){
        return new Reply(a_callType,a_seed,pNetworkReply,a_pData, a_timeoutMs);
    }
    throw Exception(a_pData,"Unable to create Network Reply object");
}


Reply* AccessManagerRaw::putRaw(const QNetworkRequest &a_request, QHttpMultiPart* a_multiPart, int a_timeoutMs, ReplyData* a_pData, int a_seed, const CallType& a_callType)
{
    QNetworkReply* pNetworkReply = m_pQtManager->put(a_request,a_multiPart);
    if(pNetworkReply){
        return new Reply(a_callType,a_seed,pNetworkReply,a_pData, a_timeoutMs);
    }
    throw Exception(a_pData,"Unable to create Network Reply object");
}


Reply* AccessManagerRaw::sendCustomRequestRaw(const QNetworkRequest& a_request, const QByteArray& a_verb, QHttpMultiPart* a_multiPart, int a_timeoutMs, ReplyData* a_pData, int a_seed, const CallType& a_callType)
{
    QNetworkReply* pNetworkReply = m_pQtManager->sendCustomRequest(a_request,a_verb,a_multiPart);
    if(pNetworkReply){
        return new Reply(a_callType,a_seed,pNetworkReply,a_pData, a_timeoutMs);
    }
    throw Exception(a_pData,"Unable to create Network Reply object");
}

#endif  //  #ifdef QTUTILS_EXTRA_REST_CALLS


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

AccessManager::AccessManager()
{
    static int snIsNotInited = 1;
    if(snIsNotInited){
        qRegisterMetaType< QTUTILS_CORE_NTDT_NSP QtUtilsNetReplyArg >( "qtutils_QtUtilsNetReplyArg" );
        snIsNotInited = 0;
    }
    m_bPendingRestart = false; 
}


AccessManagerRaw* AccessManager::accessManagerRaw()
{
    if(!m_rawManager.m_pQtManager){
        if(m_bPendingRestart){return nullptr;}
        m_rawManager.m_pQtManager = new QNetworkAccessManager();
    }
    return &m_rawManager;
}


void AccessManager::Restart()
{
    if(m_rawManager.m_pQtManager){
        QNetworkAccessManager* pCurrent = m_rawManager.m_pQtManager;
        m_rawManager.m_pQtManager = nullptr;
        m_bPendingRestart = true;
        QObject::connect(pCurrent,&QObject::destroyed,pCurrent,[this](){
            m_bPendingRestart = false;
        });
        pCurrent->deleteLater();
    }
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

#if 0
ReplyContainer::ReplyContainer()
    :
      m_pFirst(nullptr),
      m_pLast(nullptr)
{
}


ReplyContainer::~ReplyContainer()
{
    ::qtutils::invokeMethodBlocked(m_pFirst,[this](){ // function takes care when m_pFirst is null
        Clear();
    });
}


void ReplyContainer::AddNewNetworkReply(Reply* a_pReply)
{
    a_pReply->m_next = nullptr;
    a_pReply->m_prev = m_pLast;
    if(m_pLast){
        m_pLast->m_next = a_pReply;
    }
    else{
        m_pFirst = a_pReply;
    }
    m_pLast = a_pReply;
}


void ReplyContainer::RemoveNetworkReply(Reply* a_pReply)
{
    a_pReply->m_pParentContainer = nullptr;
    if(a_pReply->m_prev){a_pReply->m_prev->m_next = a_pReply->m_next;}
    if(a_pReply->m_next){a_pReply->m_next->m_prev = a_pReply->m_prev;}
    if(a_pReply==m_pFirst){m_pFirst=a_pReply->m_next;}
    if(a_pReply==m_pLast){m_pLast=a_pReply->m_prev;}
}


void ReplyContainer::Clear()
{
    Reply *pReplyTmp, *pReply = m_pFirst;
    while(pReply){
        pReply->m_pParentContainer = nullptr;
        pReplyTmp = pReply->m_next;
        pReply->Abort();
        pReply->deleteLater();
        pReply = pReplyTmp;
    }
}

#endif


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

ReplyData::~ReplyData()
{
}


QByteArray ReplyData::postData()const
{
    return QByteArray();
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

Reply::~Reply()
{
    delete m_pData;
    if(m_connDestroy){
        disconnect(m_connDestroy);
    }
    if(m_pParentContainer){
        m_pParentContainer->RemoveNetworkReply(this);
    }
    Abort();
    if(m_pNetworkReply){m_pNetworkReply->deleteLater();}
}


Reply::Reply( const CallType& a_callType, QNetworkReply* CPPUTILS_ARG_NN a_pNetworkReply, int a_timeoutMs, ReplyData* a_pData, int a_seed)
    :
    m_callType(a_callType),
    m_pNetworkReply(a_pNetworkReply),
    m_pData(a_pData)
{
    m_bHasTimeout = false;
    
	m_connDestroy = connect(m_pNetworkReply,&QObject::destroyed,this,[this](){
		deleteLater();
	});
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
    if(m_connFinished){
        QMetaObject::Connection connFinished = m_connFinished;
        m_connFinished = QMetaObject::Connection();
        disconnect(connFinished);
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
