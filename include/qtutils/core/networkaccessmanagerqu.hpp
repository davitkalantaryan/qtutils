//
// file:            networkaccessmanager.hpp
// path:			src/include/private/monitor/core/networkaccessmanager.hpp
// created on:		2021 Nov 27
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#include <qtutils/export_symbols.h>
#include <stdexcept>
#include <memory>
#include <functional>
#include <qtutils/disable_utils_warnings.h>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QVariantMap>
#include <QDateTime>
#include <QByteArray>
#ifdef QTUTILS_HTTP_SERVER_TOOLS
#include <QHttpServerRequest>
#endif

namespace qtutils { namespace network{

#define QTUTILS_NET_REPLY_HAS_AUTO_DELETE   1
#if QT_CONFIG(http) || defined(Q_OS_WASM)
#define QTUTILS_EXTRA_REST_CALLS
#else
#define QTUTILS_EXTRA_REST_CALLS
#endif


class QTUTILS_EXPORT Reply;
class QTUTILS_EXPORT ReplyData;
class QTUTILS_EXPORT AccessManager;

}}  //  namespace qtutils { namespace network{


QTUTILS_CORE_NTDT_NSP_P1


class QTUTILS_EXPORT QtUtilsNetReplyArg : public ::std::shared_ptr<::qtutils::network::Reply>
{
private:
    using ::std::shared_ptr<::qtutils::network::Reply>::shared_ptr;
    friend class Reply;
};
Q_DECLARE_METATYPE(QTUTILS_CORE_NTDT_NSP QtUtilsNetReplyArg)


QTUTILS_CORE_NTDT_NSP_P2


namespace qtutils { namespace network{

typedef ::std::function<void(void)>     TypeLateActionFunc;

struct LateAction{
    LateAction          *prev, *next;
    TypeLateActionFunc  func;
};


class QTUTILS_EXPORT Exception final : public ::std::runtime_error
{
public:
	Exception(ReplyData* a_pReplyData,const char* a_cpcWhat);
public:
	ReplyData*const m_pReplyData;
};


enum class CallType{
    None,
    Simple,
    Dependent,
    Singleton
};


class QTUTILS_EXPORT AccessManagerRaw final
{
private:
    ~AccessManagerRaw();
    AccessManagerRaw();

public:
    Reply* post(const QNetworkRequest& a_request, const QVariantMap& a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* put(const QNetworkRequest& a_request, const QVariantMap& a_data,  int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* sendCustomRequest(const QNetworkRequest& a_request, const QByteArray& a_verb, const QVariantMap&data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);

    Reply* head(const QNetworkRequest& a_request, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* get(const QNetworkRequest& a_request, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* post(const QNetworkRequest& a_request, QIODevice* a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* post(const QNetworkRequest& a_request, const QByteArray& a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* put(const QNetworkRequest& a_request, QIODevice* a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* put(const QNetworkRequest& a_request, const QByteArray& a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* deleteResource(const QNetworkRequest& a_request, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* sendCustomRequest(const QNetworkRequest& a_request, const QByteArray& a_verb, QIODevice* a_data = nullptr, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* sendCustomRequest(const QNetworkRequest& a_request, const QByteArray& a_verb, const QByteArray& a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);

#ifdef QTUTILS_EXTRA_REST_CALLS
    Reply* post(const QNetworkRequest& a_request, QHttpMultiPart* a_multiPart, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* put(const QNetworkRequest& a_request, QHttpMultiPart* a_multiPart, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* sendCustomRequest(const QNetworkRequest& a_request, const QByteArray& a_verb, QHttpMultiPart* a_multiPart, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
#endif

    Reply* postSingleton(const QNetworkRequest& a_request, const QVariantMap& a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* putSingleton(const QNetworkRequest& a_request, const QVariantMap& a_data,  int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* sendCustomRequestSingleton(const QNetworkRequest& a_request, const QByteArray& a_verb, const QVariantMap& a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);

    Reply* headSingleton(const QNetworkRequest& a_request, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* getSingleton(const QNetworkRequest& a_request, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* postSingleton(const QNetworkRequest& a_request, QIODevice* a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* postSingleton(const QNetworkRequest& a_request, const QByteArray& a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* putSingleton(const QNetworkRequest& a_request, QIODevice* a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* putSingleton(const QNetworkRequest& a_request, const QByteArray& a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* deleteResourceSingleton(const QNetworkRequest& a_request, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* sendCustomRequestSingleton(const QNetworkRequest& a_request, const QByteArray& a_verb, QIODevice* a_data = nullptr, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* sendCustomRequestSingleton(const QNetworkRequest& a_request, const QByteArray& a_verb, const QByteArray& a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);

#ifdef QTUTILS_EXTRA_REST_CALLS
    Reply* postSingleton(const QNetworkRequest& a_request, QHttpMultiPart* a_multiPart, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* putSingleton(const QNetworkRequest& a_request, QHttpMultiPart* a_multiPart, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* sendCustomRequestSingleton(const QNetworkRequest& a_request, const QByteArray& a_verb, QHttpMultiPart* a_multiPart, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
#endif

    Reply* postDependent(const QNetworkRequest& a_request, const QVariantMap& a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* putDependent(const QNetworkRequest& a_request, const QVariantMap& a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* sendCustomRequestDependent(const QNetworkRequest& a_request, const QByteArray& a_verb, const QVariantMap& a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);

    Reply* headDependent(const QNetworkRequest& a_request, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* getDependent(const QNetworkRequest& a_request, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* postDependent(const QNetworkRequest& a_request, QIODevice* a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* postDependent(const QNetworkRequest& a_request, const QByteArray& a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* putDependent(const QNetworkRequest& a_request, QIODevice* a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* putDependent(const QNetworkRequest& a_request, const QByteArray& a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* deleteResourceDependent(const QNetworkRequest& a_request, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* sendCustomRequestDependent(const QNetworkRequest& a_request, const QByteArray& a_verb, QIODevice* a_data = nullptr, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* sendCustomRequestDependent(const QNetworkRequest& a_request, const QByteArray& a_verb, const QByteArray& a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);

#ifdef QTUTILS_EXTRA_REST_CALLS
    Reply* postDependent(const QNetworkRequest& a_request, QHttpMultiPart* a_multiPart, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* putDependent(const QNetworkRequest& a_request, QHttpMultiPart* a_multiPart, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
    Reply* sendCustomRequestDependent(const QNetworkRequest& a_request, const QByteArray& a_verb, QHttpMultiPart* a_multiPart, int a_timeoutMs=-1, ReplyData* a_pData=nullptr);
#endif

private:
    AccessManagerRaw(const AccessManagerRaw&)=delete;
    AccessManagerRaw(AccessManagerRaw&&)=delete;
    AccessManagerRaw& operator=(const AccessManagerRaw&)=delete;
    AccessManagerRaw& operator=(AccessManagerRaw&&)=delete;

    Reply* CreateAndAddReply(QNetworkReply* CPPUTILS_ARG_NN a_pNetworkReply, ReplyData* a_pData, int a_timeoutMs);
    void RemoveReply(Reply* CPPUTILS_ARG_NN a_pReply);
    void AddSingleton(const TypeLateActionFunc& a_func);
    void AddDependent(const TypeLateActionFunc& a_func);

    Reply* postRaw(const QNetworkRequest& a_request, const QVariantMap& a_data, int a_timeoutMs, ReplyData* a_pData, const CallType& a_callType);
    Reply* putRaw(const QNetworkRequest& a_request, const QVariantMap& a_data, int a_timeoutMs, ReplyData* a_pData);
    Reply* sendCustomRequestRaw(const QNetworkRequest& a_request, const QByteArray& a_verb, const QVariantMap& a_data, int a_timeoutMs, ReplyData* a_pData, const CallType& a_callType);

    Reply* headRaw(const QNetworkRequest& a_request, int a_timeoutMs, ReplyData* a_pData, const CallType& a_callType);
    Reply* getRaw(const QNetworkRequest& a_request, int a_timeoutMs, ReplyData* a_pData, const CallType& a_callType);
    Reply* postRaw(const QNetworkRequest& a_request, QIODevice* a_data, int a_timeoutMs, ReplyData* a_pData, const CallType& a_callType);
    Reply* postRaw(const QNetworkRequest& a_request, const QByteArray& a_data, int a_timeoutMs, ReplyData* a_pData, const CallType& a_callType);
    Reply* putRaw(const QNetworkRequest& a_request, QIODevice* a_data, int a_timeoutMs, ReplyData* a_pData, const CallType& a_callType);
    Reply* putRaw(const QNetworkRequest& a_request, const QByteArray& a_data, int a_timeoutMs, ReplyData* a_pData, const CallType& a_callType);
    Reply* deleteResourceRaw(const QNetworkRequest& a_request, int a_timeoutMs, ReplyData* a_pData, const CallType& a_callType);
    Reply* sendCustomRequestRaw(const QNetworkRequest& a_request, const QByteArray& a_verb, QIODevice* a_data, int a_timeoutMs, ReplyData* a_pData,const CallType& a_callType);
    Reply* sendCustomRequestRaw(const QNetworkRequest& a_request, const QByteArray& a_verb, const QByteArray& a_data, int a_timeoutMs, ReplyData* a_pData, const CallType& a_callType);

#ifdef QTUTILS_EXTRA_REST_CALLS
    Reply* postRaw(const QNetworkRequest& a_request, QHttpMultiPart* a_multiPart, int a_timeoutMs, ReplyData* a_pData, const CallType& a_callType);
    Reply* putRaw(const QNetworkRequest& a_request, QHttpMultiPart* a_multiPart, int a_timeoutMs, ReplyData* a_pData, const CallType& a_callType);
    Reply* sendCustomRequestRaw(const QNetworkRequest& a_request, const QByteArray& a_verb, QHttpMultiPart* a_multiPart, int a_timeoutMs, ReplyData* a_pData, const CallType& a_callType);
#endif
    
private:
    QNetworkAccessManager*  m_pQtManager;
    Reply                   *m_pFirst;
    LateAction              *m_pFirstSingleton, *m_pLastSingleton;
    LateAction              *m_pFirstDependent, *m_pLastDependent;
    int                     m_countOfCorelated;
    
    friend class AccessManager;
};


class QTUTILS_EXPORT AccessManager final
{
public:
    ~AccessManager();
    AccessManager(bool a_bQuitAppInDestuctor=nullptr);
    AccessManagerRaw*   accessManagerRaw();
    void   Restart();

private:
    AccessManager(const AccessManager&)=delete;
    AccessManager(AccessManager&&)=delete;
    AccessManager& operator=(const AccessManager&)=delete;
    AccessManager& operator=(AccessManager&&)=delete;
    
private:
    AccessManagerRaw	m_rawManager;
    bool				m_bPendingRestart;
    bool				m_bQuitAppInDestuctor;
};


class QTUTILS_EXPORT ReplyData
{
public:
    virtual ~ReplyData();
    virtual QByteArray postData()const;
};


class QTUTILS_EXPORT Reply final : public QObject
{
    Q_OBJECT

public:
    void Abort();
    QNetworkReply* operator->()const;
    QNetworkReply* qtNetworkReply()const;
    ReplyData* data()const;
    void ReplaceData(ReplyData* a_pData); // this will be used to replace by null to prevent delete
    bool hasTimeout()const;
    
private:
signals:
    void finished(QTUTILS_CORE_NTDT_NSP QtUtilsNetReplyArg);

private:
    ~Reply() override;  // don't panic it is deleted automatically :)
    Reply( const CallType& a_callType, QNetworkReply* CPPUTILS_ARG_NN a_pNetworkReply, ReplyData* a_pData, int a_timeoutMs);
    Reply() = delete;
    Reply(const Reply&) = delete;
    Reply(Reply&&) = delete;
    Reply& operator=(const Reply&) = delete;
    Reply& operator=(Reply&&) = delete;

private:
    const CallType          m_callType;
    Reply                   *m_prev, *m_next;
    QTimer                  m_timeoutTimer;
    QNetworkReply* const	m_pNetworkReply;
    ReplyData*              m_pData;
    QMetaObject::Connection	m_connFinished;
    QMetaObject::Connection	m_connTimeout;
    QMetaObject::Connection	m_connDestroy;
    bool                    m_bHasTimeout;
    bool                    m_bWaitBeforeAppExit;
    
    friend class AccessManagerRaw;
};


QTUTILS_EXPORT void PrepareHeadersRaw(const QByteArray& a_contTypeHeader,QNetworkRequest* a_pRequet, const QString& a_agent);
QTUTILS_EXPORT void PrepareJsonHeaders(QNetworkRequest* a_pRequet, const QString& a_agent);
QTUTILS_EXPORT void PrepareJsonHeadersWithAuth(QNetworkRequest* a_pRequet, const QString& a_authToken, const QString& a_agent);
QTUTILS_EXPORT void PrepareMPartHeaders(QNetworkRequest* a_pRequet, const QString& a_agent);
QTUTILS_EXPORT void PrepareMPartHeadersWithAuth(QNetworkRequest* a_pRequet, const QString& a_authToken,const QString& a_agent);
QTUTILS_EXPORT void ErrorByteArray(const QNetworkReply::NetworkError&,const ::qtutils::network::Reply& a_replyHandlerIn, QByteArray* CPPUTILS_IN_OUT a_pData);
QTUTILS_EXPORT QString CorectUrl(const QString& a_url);
QTUTILS_EXPORT QString NetworkErrorCodeString(const QNetworkReply::NetworkError& a_errorCode);
#ifdef QTUTILS_HTTP_SERVER_TOOLS
QTUTILS_EXPORT QByteArray HttpRequestMethodToByteArray(const QHttpServerRequest::Method& a_method);
#endif


}}  //  namespace qtutils { namespace network{
