//
// file:            websockscontainer.hpp
// path:			include/focust/core/websockscontainer.hpp
// created on:		2022 Jan 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once


#include <focust/core/focust_internal_header.h>
#include <stddef.h>
#include <focust/core/disable_utils_warnings.h>
#include <QWebSocket>
#include <QByteArray>



namespace focust{

class FOCUST_DLL_PRIVATE WebSockContainer_p;


class FOCUST_EXPORT WebSockContainer final
{
public:
    WebSockContainer(size_t maxNumberOfSocketsInOneThread);
    WebSockContainer(const WebSockContainer&)=delete;
    WebSockContainer(WebSockContainer&&)=delete;
    ~WebSockContainer();
    WebSockContainer& operator=(const WebSockContainer&)=delete;
    WebSockContainer& operator=(WebSockContainer&&)=delete;

    void AddNewWebSocket(QWebSocket* a_pSocket, void* a_pUserData);
    void RemoveWebSocket(QWebSocket* a_pSocket);
    void SendInfo(const QByteArray& a_info);
    static void* getUserData(QWebSocket* a_pSocket);
    void  MoveFromHereToOther(WebSockContainer* a_pOther, QWebSocket* a_pSocket);

private:
    WebSockContainer_p*const m_pContData;
};

}  // namespace focust{
