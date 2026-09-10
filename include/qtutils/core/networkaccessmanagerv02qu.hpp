//
// repo:            qtutils
// file:            networkaccessmanagerv02.hpp
// path:			include/qtutils/networkaccessmanagerv02qu.hpp
// created on:		2026 Sep 05
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#pragma once

#include <qtutils/export_symbols.h>
#include <cinternal/disable_compiler_warnings.h>
#include <functional>
#include <memory>
#include <qtutils/disable_utils_warnings.h>
#include <QByteArray>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#ifdef QTUTILS_EXTRA_REST_CALLS
#include <QHttpMultiPart>
#endif
#include <cinternal/undisable_compiler_warnings.h>


class QTUTILS_EXPORT QuCoreNetReplyArgV02;


namespace qtutils { namespace core{ namespace network{


class QTUTILS_EXPORT Reply;
class CPPUTILS_DLL_PRIVATE Reply_p;
class CPPUTILS_DLL_PRIVATE AccessManager_p;
typedef ::std::function<QNetworkReply*(QNetworkAccessManager*)>    TypeRestCall;


class QTUTILS_EXPORT AccessManager final
{
public:
    ~AccessManager();
    AccessManager();

    Reply* AnyRestCall(const TypeRestCall& a_restCallFnc);
    Reply* AnyRestCall(int a_timeoutMs, const TypeRestCall& a_restCallFnc);
    void RestartNetAccessManaget();
    QNetworkAccessManager* pQtNetAccessMngr()const noexcept;
    int exitTimeoutMs()const noexcept;
    void SetExitTimeoutMs(int a_exitTimeoutMs)noexcept;

private:
    AccessManager_p* const  m_data_p;

private:
    AccessManager(const AccessManager&)=delete;
    AccessManager(AccessManager&&)=delete;
    AccessManager& operator=(const AccessManager&)=delete;
    AccessManager& operator=(AccessManager&&)=delete;
};


class QTUTILS_EXPORT Reply final : public QObject
{
    Q_OBJECT
public:
    void Abort();
    QNetworkReply* qtNetworkReply()const noexcept;
    int timeoutMs()const noexcept;
    bool isTimedOut()const noexcept;
    void MakeThisCallBlockExit()noexcept;

private:
signals:
    void finished(QuCoreNetReplyArgV02);

public:
    ~Reply() override;  // don't panic it is deleted automatically :)
private:
    Reply(AccessManager_p* CPPUTILS_ARG_NN a_pAccsMngr, QNetworkReply* CPPUTILS_ARG_NN a_pQtNetReply, int a_timeoutMs);

private:
    Reply_p* const      m_data_p;

private:
    Reply(const Reply&) = delete;
    Reply(Reply&&) = delete;
    Reply& operator=(const Reply&) = delete;
    Reply& operator=(Reply&&) = delete;
    friend class AccessManager;
};


QTUTILS_EXPORT void PrepareHeadersRaw(const QByteArray& a_contTypeHeader,QNetworkRequest* a_pRequet, const QString& a_agent);
QTUTILS_EXPORT void PrepareJsonHeaders(QNetworkRequest* a_pRequet, const QString& a_agent);
QTUTILS_EXPORT void PrepareJsonHeadersWithAuth(QNetworkRequest* a_pRequet, const QString& a_authToken, const QString& a_agent);
QTUTILS_EXPORT void PrepareMPartHeaders(QNetworkRequest* a_pRequet, const QString& a_agent);
QTUTILS_EXPORT void PrepareMPartHeadersWithAuth(QNetworkRequest* a_pRequet, const QString& a_authToken,const QString& a_agent);
QTUTILS_EXPORT void ErrorByteArray(const QNetworkReply::NetworkError&,const ::qtutils::core::network::Reply& a_replyHandlerIn, QByteArray* CPPUTILS_IN_OUT a_pData);
QTUTILS_EXPORT QString CorectUrl(const QString& a_url);
QTUTILS_EXPORT QString NetworkErrorCodeString(const QNetworkReply::NetworkError& a_errorCode);
#ifdef QTUTILS_HTTP_SERVER_TOOLS
QTUTILS_EXPORT QByteArray HttpRequestMethodToByteArray(const QHttpServerRequest::Method& a_method);
#endif


}}}  //  namespace qtutils { namespace core{ namespace network{


class QTUTILS_EXPORT QuCoreNetReplyArgV02 final : public ::std::shared_ptr<::qtutils::core::network::Reply>
{
private:
    using ::std::shared_ptr<::qtutils::core::network::Reply>::shared_ptr;
    friend class ::qtutils::core::network::Reply_p;
};
Q_DECLARE_METATYPE(QuCoreNetReplyArgV02)
