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
#define QTUTILS_NET_NO_SEED     static_cast<unsigned int>(-1)

#if QT_CONFIG(http) || defined(Q_OS_WASM)
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

typedef ::std::function<QNetworkReply*(void)>     TypeLateActionFunc;

struct SReplyListItem{
    Reply   *prev, *next;
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
    SimpleDropSeed,
    SimpleKeepSeed,
    DependentDropSeed,
    DependentKeepSeed,
    SingletonDropSeed,
    SingletonKeepSeed
};


class QTUTILS_EXPORT AccessManager final
{
public:
    ~AccessManager();
    AccessManager();

    void Restart();
    void QuitApp(const ::std::function<void(void)>& a_destruct);

    // simple apps
    Reply* post(const QNetworkRequest& a_request, const QByteArray& a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* post(const QNetworkRequest& a_request, QIODevice* a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* post(const QNetworkRequest& a_request, const QVariantMap& a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);

    Reply* put(const QNetworkRequest& a_request, const QByteArray& a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* put(const QNetworkRequest& a_request, QIODevice* a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* put(const QNetworkRequest& a_request, const QVariantMap& a_data,  int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);

    Reply* sendCustomRequest(const QNetworkRequest& a_request, const QByteArray& a_verb, const QByteArray& a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* sendCustomRequest(const QNetworkRequest& a_request, const QByteArray& a_verb, QIODevice* a_data = nullptr, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* sendCustomRequest(const QNetworkRequest& a_request, const QByteArray& a_verb, const QVariantMap& a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);

    Reply* get(const QNetworkRequest& a_request, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* head(const QNetworkRequest& a_request, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* deleteResource(const QNetworkRequest& a_request, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);

#ifdef QTUTILS_EXTRA_REST_CALLS
    Reply* post(const QNetworkRequest& a_request, QHttpMultiPart* a_pMultiPart, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* put(const QNetworkRequest& a_request, QHttpMultiPart* a_pMultiPart, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* sendCustomRequest(const QNetworkRequest& a_request, const QByteArray& a_verb, QHttpMultiPart* a_pMultiPart, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
#endif

    // singletons
    Reply* postSingleton(const QNetworkRequest& a_request, const QByteArray& a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* postSingleton(const QNetworkRequest& a_request, QIODevice* a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* postSingleton(const QNetworkRequest& a_request, const QVariantMap& a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);

    Reply* putSingleton(const QNetworkRequest& a_request, const QByteArray& a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* putSingleton(const QNetworkRequest& a_request, QIODevice* a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* putSingleton(const QNetworkRequest& a_request, const QVariantMap& a_data,  int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);

    Reply* sendCustomRequestSingleton(const QNetworkRequest& a_request, const QByteArray& a_verb, const QByteArray& a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* sendCustomRequestSingleton(const QNetworkRequest& a_request, const QByteArray& a_verb, QIODevice* a_data = nullptr, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* sendCustomRequestSingleton(const QNetworkRequest& a_request, const QByteArray& a_verb, const QVariantMap& a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);

    Reply* getSingleton(const QNetworkRequest& a_request, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* headSingleton(const QNetworkRequest& a_request, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* deleteResourceSingleton(const QNetworkRequest& a_request, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);

#ifdef QTUTILS_EXTRA_REST_CALLS
    Reply* postSingleton(const QNetworkRequest& a_request, QHttpMultiPart* a_pMultiPart, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* putSingleton(const QNetworkRequest& a_request, QHttpMultiPart* a_pMultiPart, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* sendCustomRequestSingleton(const QNetworkRequest& a_request, const QByteArray& a_verb, QHttpMultiPart* a_pMultiPart, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
#endif

    // dependents
    Reply* postDependent(const QNetworkRequest& a_request, const QByteArray& a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* postDependent(const QNetworkRequest& a_request, QIODevice* a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* postDependent(const QNetworkRequest& a_request, const QVariantMap& a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);

    Reply* putDependent(const QNetworkRequest& a_request, const QByteArray& a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* putDependent(const QNetworkRequest& a_request, QIODevice* a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* putDependent(const QNetworkRequest& a_request, const QVariantMap& a_data,  int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);

    Reply* sendCustomRequestDependent(const QNetworkRequest& a_request, const QByteArray& a_verb, const QByteArray& a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* sendCustomRequestDependent(const QNetworkRequest& a_request, const QByteArray& a_verb, QIODevice* a_data = nullptr, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* sendCustomRequestDependent(const QNetworkRequest& a_request, const QByteArray& a_verb, const QVariantMap& a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);

    Reply* getDependent(const QNetworkRequest& a_request, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* headDependent(const QNetworkRequest& a_request, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* deleteResourceDependent(const QNetworkRequest& a_request, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);

#ifdef QTUTILS_EXTRA_REST_CALLS
    Reply* postDependent(const QNetworkRequest& a_request, QHttpMultiPart* a_pMultiPart, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* putDependent(const QNetworkRequest& a_request, QHttpMultiPart* a_pMultiPart, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* sendCustomRequestDependent(const QNetworkRequest& a_request, const QByteArray& a_verb, QHttpMultiPart* a_pMultiPart, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
#endif

    // any
    Reply* postAny(const CallType& a_callType, const QNetworkRequest& a_request, const QByteArray& a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* postAny(const CallType& a_callType, const QNetworkRequest& a_request, QIODevice* a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* postAny(const CallType& a_callType, const QNetworkRequest& a_request, const QVariantMap& a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);

    Reply* putAny(const CallType& a_callType, const QNetworkRequest& a_request, const QByteArray& a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* putAny(const CallType& a_callType, const QNetworkRequest& a_request, QIODevice* a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* putAny(const CallType& a_callType, const QNetworkRequest& a_request, const QVariantMap& a_data,  int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);

    Reply* sendCustomRequestAny(const CallType& a_callType, const QNetworkRequest& a_request, const QByteArray& a_verb, const QByteArray& a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* sendCustomRequestAny(const CallType& a_callType, const QNetworkRequest& a_request, const QByteArray& a_verb, QIODevice* a_data = nullptr, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* sendCustomRequestAny(const CallType& a_callType, const QNetworkRequest& a_request, const QByteArray& a_verb, const QVariantMap& a_data, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);

    Reply* getAny(const CallType& a_callType, const QNetworkRequest& a_request, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* headAny(const CallType& a_callType, const QNetworkRequest& a_request, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* deleteResourceAny(const CallType& a_callType, const QNetworkRequest& a_request, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);

#ifdef QTUTILS_EXTRA_REST_CALLS
    Reply* postAny(const CallType& a_callType, const QNetworkRequest& a_request, QHttpMultiPart* a_pMultiPart, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* putAny(const CallType& a_callType, const QNetworkRequest& a_request, QHttpMultiPart* a_pMultiPart, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
    Reply* sendCustomRequestAny(const CallType& a_callType, const QNetworkRequest& a_request, const QByteArray& a_verb, QHttpMultiPart* a_pMultiPart, int a_timeoutMs=-1, ReplyData* a_pData=nullptr, unsigned int a_seed=QTUTILS_NET_NO_SEED);
#endif

private:
    AccessManager(const AccessManager&)=delete;
    AccessManager(AccessManager&&)=delete;
    AccessManager& operator=(const AccessManager&)=delete;
    AccessManager& operator=(AccessManager&&)=delete;

    Reply* CreateAndAddReply(const CallType& a_callType, unsigned int a_seed, int a_timeoutMs, ReplyData* a_pData, const TypeLateActionFunc& a_func);
    void RunReplyIfAllowedRaw(Reply* CPPUTILS_ARG_NN a_pReply);
    void RemoveReply(Reply* CPPUTILS_ARG_NN a_pReply);
    void AddSimple(Reply* CPPUTILS_ARG_NN a_pReply);
    void AddSingleton(Reply* CPPUTILS_ARG_NN a_pReply);
    void AddDependent(Reply* CPPUTILS_ARG_NN a_pReply);
    bool SeedCheckAndAdd(Reply* CPPUTILS_ARG_NN a_pReply);
    void FindAndRemoveSeed(unsigned int a_seed, const CallType& a_type);
    Reply* findSeedRaw(unsigned int a_seed, unsigned int* CPPUTILS_ARG_NN a_nIndex_p)const;
    
private:
    typedef Reply*       SeedDataPtr;
    QNetworkAccessManager*          m_pQtManager;
    SeedDataPtr* const              m_seedData_p;
    Reply                           *m_pFirstWaiting;
    Reply                           *m_pFirstSimple, *m_pLastSimple;
    Reply                           *m_pFirstSingleton, *m_pLastSingleton;
    Reply                           *m_pFirstDependent, *m_pLastDependent;
    bool                            m_bShouldRun;
    bool                            m_bPendingRestart;
    bool                            m_bQuitAppInDestuctor;
    ::std::function<void(void)>     m_destruct;
};


class QTUTILS_EXPORT ReplyData
{
public:
    virtual ~ReplyData();
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
    bool blockAppExit()const;
    void SetBlockAppExit(bool a_bBlockAppExit);
    
private:
signals:
    void finished(QTUTILS_CORE_NTDT_NSP QtUtilsNetReplyArg);

private:
    ~Reply() override;  // don't panic it is deleted automatically :)
    Reply(const CallType& a_callType, unsigned int a_seed, int a_timeoutMs, ReplyData* a_pData, const TypeLateActionFunc& a_func);
    Reply() = delete;
    Reply(const Reply&) = delete;
    Reply(Reply&&) = delete;
    Reply& operator=(const Reply&) = delete;
    Reply& operator=(Reply&&) = delete;
    void RunFunction();
    void FromDestructorOfAccessManager();

private:
    const CallType              m_callType;
    const unsigned int          m_seed;
    const int                   m_timeout;
    const TypeLateActionFunc    m_func;
    SReplyListItem              m_itr;
    SReplyListItem              m_sed;
    QTimer                      m_timeoutTimer;
    QNetworkReply*              m_pNetworkReply;
    ReplyData*                  m_pData;
    QMetaObject::Connection     m_connFinished;
    QMetaObject::Connection     m_connTimeout;
    QMetaObject::Connection     m_connDestroy;
    bool                        m_bHasTimeout;
    bool                        m_bBlockAppExit;
    bool                        m_bFinishedEmited;
    bool                        m_bAdded;
    
    friend class AccessManager;
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
