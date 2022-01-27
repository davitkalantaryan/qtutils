//
// file:            focust_websockscontainer.cpp
// path:			src/core/focust_websockscontainer.cpp
// created on:		2022 Jan 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#include <qtutils/core/websockscontainer.hpp>
#include <qtutils/core/threadobject.hpp>
#include <qtutils/core/threadls.hpp>
#include <cpputils/inscopecleaner.hpp>
#include <cpputils/unnamedsemaphore.hpp>
#include <list>
#include <mutex>
#include <assert.h>
#include <qtutils/disable_utils_warnings.h>
#include <QThread>
#include <QDebug>


namespace qtutils{

class CPPUTILS_DLL_PRIVATE WebSocketN;
typedef ::std::list<WebSocketN*>  ListSock;

class CPPUTILS_DLL_PRIVATE WebSocketN final : public QObject
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


class CPPUTILS_DLL_PRIVATE WebSockContainer_p
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
        ::std::lock_guard<::std::mutex> aGuard(m_pContData->m_mutexSocks);
        m_pContData->m_socks.erase(pNewSock->m_iter);
        //a_pSocket->close();
        //pNewSock->deleteLater();
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


} // namespace qtutils{
