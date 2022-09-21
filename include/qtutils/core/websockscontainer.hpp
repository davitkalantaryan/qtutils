//
// file:            websockscontainer.hpp
// path:			include/qtutils/core/websockscontainer.hpp
// created on:		2022 Jan 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once


#include <qtutils/export_symbols.h>
#include <stddef.h>
#include <qtutils/disable_utils_warnings.h>
#include <QWebSocket>
#include <QByteArray>



namespace qtutils{

class CPPUTILS_DLL_PRIVATE WebSockContainer_p;


class QTUTILS_EXPORT WebSockContainer final
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

}  // namespace qtutils{
