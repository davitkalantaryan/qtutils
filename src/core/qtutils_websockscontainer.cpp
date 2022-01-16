//
// file:            focust_websockscontainer.cpp
// path:			src/core/focust_websockscontainer.cpp
// created on:		2022 Jan 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#include <focust/core/websockscontainer.hpp>
#include <focust/core/threadobject.hpp>
#include <focust/core/threadls.hpp>
#include <cpputils/inscopecleaner.hpp>
#include <cpputils/unnamedsemaphore.hpp>
#include <list>
#include <mutex>
#include <assert.h>
#include <focust/core/disable_utils_warnings.h>
#include <QThread>
#include <QDebug>


namespace focust{

class FOCUST_DLL_PRIVATE WebSocketN;
typedef ::std::list<WebSocketN*>  ListSock;

class FOCUST_DLL_PRIVATE WebSocketN final : public QObject
{
public:
    WebSocketN(QWebSocket* a_pSock, void* a_pUserData);
    WebSocketN(const WebSocketN&)=delete;
    WebSocketN(WebSocketN&&)=delete;
    WebSocketN& operator=(const WebSocketN&)=delete;
    WebSocketN& operator=(WebSocketN&&)=delete;
public:
    QWebSocket*const            m_pSock;
    void*const                  m_pUsDt;
    ListSock::const_iterator    m_iter;
};


class FOCUST_DLL_PRIVATE WebSockContainer_p
{
public:
    WebSockContainer_p();
    ~WebSockContainer_p();
public:
    QObject*    m_pObject;
    ListSock    m_socks;
    ::std::mutex    m_mutexSocks;
};


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

WebSockContainer::WebSockContainer(size_t a_maxNumberOfSocketsInOneThread)
    :
      m_pContData(new WebSockContainer_p())
{
    static_cast<void>(a_maxNumberOfSocketsInOneThread);
}


WebSockContainer::~WebSockContainer()
{
    delete m_pContData;
}


void* WebSockContainer::getUserData(QWebSocket* a_pSocket)
{
    WebSocketN* pNewSock = static_cast<WebSocketN*>(a_pSocket->parent());
    return pNewSock->m_pUsDt;
}


void WebSockContainer::AddNewWebSocket(QWebSocket* a_pSocket, void* a_pUserData)
{
    if(!m_pContData->m_pObject){
        m_pContData->m_pObject = new QObject;
    }
    QMetaObject::invokeMethod(m_pContData->m_pObject,[this, a_pSocket,a_pUserData](){
        ::std::lock_guard<::std::mutex> aGuard(m_pContData->m_mutexSocks);
        WebSocketN* pSockNew = new WebSocketN(a_pSocket,a_pUserData);
        m_pContData->m_socks.push_front(pSockNew);
        pSockNew->m_iter = m_pContData->m_socks.begin();
    });
}


void WebSockContainer::RemoveWebSocket(QWebSocket* a_pSocket)
{
    QMetaObject::invokeMethod(m_pContData->m_pObject,[this, a_pSocket](){
        WebSocketN* pNewSock = static_cast<WebSocketN*>(a_pSocket->parent());
        a_pSocket->close();
        ::std::lock_guard<::std::mutex> aGuard(m_pContData->m_mutexSocks);
        m_pContData->m_socks.erase(pNewSock->m_iter);
        pNewSock->deleteLater();
    });
}


void WebSockContainer::MoveFromHereToOther(WebSockContainer* a_pOther, QWebSocket* a_pSocket)
{
    static_cast<void>(a_pOther);
    static_cast<void>(a_pSocket);
    QMetaObject::invokeMethod(m_pContData->m_pObject,[this, a_pOther, a_pSocket](){
        WebSocketN* pNewSock = static_cast<WebSocketN*>(a_pSocket->parent());

        {
            ::std::lock_guard<::std::mutex> aGuardThis(m_pContData->m_mutexSocks);
            m_pContData->m_socks.erase(pNewSock->m_iter);
        }

        if(!a_pOther->m_pContData->m_pObject){
            a_pOther->m_pContData->m_pObject = new QObject;
        }

        {
            ::std::lock_guard<::std::mutex> aGuardOther(a_pOther->m_pContData->m_mutexSocks);
            a_pOther->m_pContData->m_socks.push_front(pNewSock);
            pNewSock->m_iter = a_pOther->m_pContData->m_socks.begin();
        }
    });
}


void WebSockContainer::SendInfo(const QByteArray& a_info)
{
    QMetaObject::invokeMethod(m_pContData->m_pObject,[this, a_info](){
        ::std::lock_guard<::std::mutex> aGuard(m_pContData->m_mutexSocks);
        for(const auto& pNewSock : m_pContData->m_socks){
            pNewSock->m_pSock->sendBinaryMessage(a_info);
        }
    });
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

WebSockContainer_p::WebSockContainer_p()
    :
      m_pObject(nullptr)
{
}


WebSockContainer_p::~WebSockContainer_p()
{
    if(m_pObject){
        QMetaObject::invokeMethod(m_pObject,[this](){
            delete m_pObject;
            m_pObject = nullptr;
        });
    }
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

WebSocketN::WebSocketN(QWebSocket* a_pSock, void* a_pUserData)
    :
      m_pSock(a_pSock),
      m_pUsDt(a_pUserData)
{
    a_pSock->setParent(this);
}


} // namespace focust{


#if 0

namespace focust{


class FOCUST_DLL_PRIVATE WebSocketN;
class FOCUST_DLL_PRIVATE ThreadData;
typedef ::std::list<WebSocketN*>  ListSock;


class FOCUST_DLL_PRIVATE WebSocketN final : public QObject
{
public:
    WebSocketN(QWebSocket* a_pSock, void* a_pUserData);
    WebSocketN(const WebSocketN&)=delete;
    WebSocketN(WebSocketN&&)=delete;
    WebSocketN& operator=(const WebSocketN&)=delete;
    WebSocketN& operator=(WebSocketN&&)=delete;
public:
    QWebSocket*const            m_pSock;
    ThreadData*                 m_pThr;
    void*const                  m_pUsDt;
    ListSock::const_iterator    m_iter;
};


class FOCUST_DLL_PRIVATE ThreadData final
{
public:
    ThreadData      *prev, *next;
    ThreadLS        thr;
    ListSock        socks;
    ::std::mutex    mutexForSocks;
};


class FOCUST_DLL_PRIVATE WebSockContainer_p
{
public:
    WebSockContainer_p(size_t a_maxNumberOfSocketsInOneThread);

    inline void FindOrCreateNewThreadObjNoLock();
    inline void RemoveThreadObjNoLock(ThreadData* a_pThreadObj);
    inline void RemoveWebSocketSockThread(QWebSocket* a_pSocket);
    inline void AddNewWebSocket(WebSocketN* a_pSocketN);

public:
    const size_t    m_maxNumberOfSocketsInOneThread;
    ThreadData*     m_pFirstThr;
    ThreadData*     m_pLastThr;
    ThreadData*     m_pCurThr;
    ::std::mutex    m_mutexForThreads;
};


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

WebSockContainer::WebSockContainer(size_t a_maxNumberOfSocketsInOneThread)
    :
      m_pContData(new WebSockContainer_p(a_maxNumberOfSocketsInOneThread))
{
}


WebSockContainer::~WebSockContainer()
{
    delete m_pContData;
}


void* WebSockContainer::getUserData(QWebSocket* a_pSocket)
{
    WebSocketN* pNewSock = static_cast<WebSocketN*>(a_pSocket->parent());
    return pNewSock->m_pUsDt;
}


void WebSockContainer::AddNewWebSocket(QWebSocket* a_pSocket, void* a_pUserData)
{
    WebSocketN* pNewSock = new WebSocketN(a_pSocket,a_pUserData);
    m_pContData->AddNewWebSocket(pNewSock);
}


void WebSockContainer::RemoveWebSocket(QWebSocket* a_pSocket)
{
    QMetaObject::invokeMethod(a_pSocket,[this, a_pSocket](){
        m_pContData->RemoveWebSocketSockThread(a_pSocket);
    });
}


void WebSockContainer::MoveFromHereToOther(WebSockContainer* a_pOther, QWebSocket* a_pSocket)
{
    WebSocketN* pNewSock = nullptr;

    {
        ::std::lock_guard<::std::mutex> aGuard(m_pContData->m_mutexForThreads);
        pNewSock = static_cast<WebSocketN*>(a_pSocket->parent());
        ThreadData* pThr = pNewSock->m_pThr;

        {
            ::std::lock_guard<::std::mutex> aGuardIn(pThr->mutexForSocks);
             pThr->socks.erase(pNewSock->m_iter);
             m_pContData->m_pCurThr = pThr;
             if(pThr->socks.size()<1){
                 m_pContData->RemoveThreadObjNoLock(pThr);
             }
        }
    }

    //a_pOther->m_pContData->AddNewWebSocket(pNewSock);
    QMetaObject::invokeMethod(pNewSock,[a_pOther, pNewSock](){
        a_pOther->m_pContData->AddNewWebSocket(pNewSock);
    });
}


void WebSockContainer::SendInfo(const QByteArray& a_info)
{
    ::std::lock_guard<::std::mutex> aGuard(m_pContData->m_mutexForThreads);
    ThreadData* pThrObj = m_pContData->m_pFirstThr;

    while(pThrObj){
        QMetaObject::invokeMethod(pThrObj->thr.thrObj(),[pThrObj,a_info](){
            ::std::lock_guard<::std::mutex> aGuardIn(pThrObj->mutexForSocks);
            for(const auto& pNewSock : pThrObj->socks){
                pNewSock->m_pSock->sendBinaryMessage(a_info);
            }
        });
        pThrObj = pThrObj->next;
    }
}



/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

WebSockContainer_p::WebSockContainer_p(size_t a_maxNumberOfSocketsInOneThread)
    :
      m_maxNumberOfSocketsInOneThread(a_maxNumberOfSocketsInOneThread),
      m_pFirstThr(nullptr),
      m_pLastThr(nullptr),
      m_pCurThr(nullptr)
{
}


inline void WebSockContainer_p::FindOrCreateNewThreadObjNoLock()
{
    ThreadData* pNewThrObj = m_pFirstThr;
    while(pNewThrObj){
        if(pNewThrObj->socks.size()<m_maxNumberOfSocketsInOneThread){
            m_pCurThr = pNewThrObj;
            return;
        }
    }

    pNewThrObj = new ThreadData();

    if(m_pLastThr){m_pLastThr->prev = pNewThrObj;}
    else{m_pFirstThr = pNewThrObj;}
    pNewThrObj->prev = m_pLastThr;
    m_pLastThr = pNewThrObj;
    m_pCurThr = pNewThrObj;
}


inline void WebSockContainer_p::RemoveThreadObjNoLock(ThreadData* a_pThreadObj)
{
    if(a_pThreadObj->prev){a_pThreadObj->prev->next = a_pThreadObj->next;}
    if(a_pThreadObj->next){a_pThreadObj->next->prev = a_pThreadObj->prev;}
    if(a_pThreadObj==m_pFirstThr){m_pFirstThr=a_pThreadObj->next;}
    if(a_pThreadObj==m_pLastThr){m_pLastThr=a_pThreadObj->prev;}
    if(a_pThreadObj==m_pCurThr){m_pCurThr=a_pThreadObj->next;}
}


inline void WebSockContainer_p::AddNewWebSocket(WebSocketN* a_pSocketN)
{
    ::std::lock_guard<::std::mutex> aGuard(m_mutexForThreads);

    if(!m_pCurThr||(m_pCurThr->socks.size()>=m_maxNumberOfSocketsInOneThread)){
        FindOrCreateNewThreadObjNoLock();
    }
    assert(m_pCurThr);
    a_pSocketN->moveToThread(&(m_pCurThr->thr));

    {
        ::std::lock_guard<::std::mutex> aGuardIn(m_pCurThr->mutexForSocks);
        a_pSocketN->m_pThr = m_pCurThr;
        m_pCurThr->socks.push_front(a_pSocketN);
        a_pSocketN->m_iter = m_pCurThr->socks.begin();
    }
}


inline void WebSockContainer_p::RemoveWebSocketSockThread(QWebSocket* a_pSocket)
{
    ::std::lock_guard<::std::mutex> aGuard(m_mutexForThreads);
    WebSocketN* pNewSock = static_cast<WebSocketN*>(a_pSocket->parent());
    ThreadData* pThr = pNewSock->m_pThr;

    {
        ::std::lock_guard<::std::mutex> aGuardIn(pThr->mutexForSocks);
         pThr->socks.erase(pNewSock->m_iter);
         m_pCurThr = pThr;
         if(pThr->socks.size()<1){
             RemoveThreadObjNoLock(pThr);
         }
         pNewSock->deleteLater();
    }
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

WebSocketN::WebSocketN(QWebSocket* a_pSock, void* a_pUserData)
    :
      m_pSock(a_pSock),
      m_pThr(nullptr),
      m_pUsDt(a_pUserData)
{
    a_pSock->setParent(this);
}


}  // namespace focust{

#endif  // #if 0
