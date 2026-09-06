//
// repo:            qtutils
// file:            qtutils_core_networkaccessmanagerv02.cpp
// path:			src/core/qtutils_core_networkaccessmanagerv02.cpp
// created on:		2026 Sep 05
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//


#include <qtutils/core/networkaccessmanagerv02qu.hpp>
#include <qtutils/core/utils.hpp>
#include <cinternal/bistateflags.h>
#include <cinternal/disable_compiler_warnings.h>
#include <stdexcept>
#include <qtutils/disable_utils_warnings.h>
#include <QMetaObject>
#include <QTimer>
#include <cinternal/undisable_compiler_warnings.h>


namespace qtutils { namespace core{ namespace network{


class CPPUTILS_DLL_PRIVATE RaiiDeleter
{
public:
    typedef ::std::function<void(void)> TypeDeleter;
public:
    ~RaiiDeleter();
    RaiiDeleter(const TypeDeleter& a_dltr);
private:
    const TypeDeleter   m_dltr;
private:
    RaiiDeleter(const RaiiDeleter&)=delete;
    RaiiDeleter(RaiiDeleter&&)=delete;
    RaiiDeleter& operator=(const RaiiDeleter&)=delete;
    RaiiDeleter& operator=(RaiiDeleter&&)=delete;
};


class CPPUTILS_DLL_PRIVATE Reply_p
{
public:
    Reply* const                    m_pParent;
    QuCoreNetReplyArgV02               m_finishArg;
    AccessManager_p*                m_pParentAccessMngr = nullptr;
    QNetworkReply*                  m_pQtNetReply = nullptr;
    const int                       m_timeoutMs;
    QMetaObject::Connection         m_connFinished;
    QMetaObject::Connection         m_connTimeout;
    QMetaObject::Connection         m_connDestroy;
    QTimer                          m_timeoutTimer;
    Reply_p                         *m_prev, *m_next;
    CPPUTILS_BISTATE_FLAGS_UN(
        hasTimeout,
        finishEmitted,
        abortCalled
    )m_flagsBS;

public:
    ~Reply_p();
    Reply_p(Reply* CPPUTILS_ARG_NN a_pParent, int a_timeoutMs);

    inline void DisconnectAllConnectionsAndRetIfDisconnectedInline();
    inline void AbortInlineRaw();
    inline void AbortInline2();
    void ConnectSignalsAndStartTimer();

private:
    Reply_p(const Reply_p&)=delete;
    Reply_p(Reply_p&&)=delete;
    Reply_p& operator=(const Reply_p&)=delete;
    Reply_p& operator=(Reply_p&&)=delete;
};


class CPPUTILS_DLL_PRIVATE AccessManager_p final
{
public:
    QNetworkAccessManager*  m_pQtNetAccessManager;
    Reply_p*                m_pFirst;

public:
    ~AccessManager_p();
    AccessManager_p();

    inline void DestroyQtNetAccessManagerInline();

private:
    AccessManager_p(const AccessManager_p&)=delete;
    AccessManager_p(AccessManager_p&&)=delete;
    AccessManager_p& operator=(const AccessManager_p&)=delete;
    AccessManager_p& operator=(AccessManager_p&&)=delete;
};


/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

static inline void DisconnectMetaConnectionInline(QMetaObject::Connection* CPPUTILS_ARG_NN a_pMetaConn){
    if(*a_pMetaConn){
        QObject::disconnect(*a_pMetaConn);
        *a_pMetaConn = QMetaObject::Connection();
    }  //  if(*a_pMetaConn){
}


inline void Reply_p::DisconnectAllConnectionsAndRetIfDisconnectedInline(){
    DisconnectMetaConnectionInline(&m_connDestroy);
    DisconnectMetaConnectionInline(&m_connTimeout);
    DisconnectMetaConnectionInline(&m_connFinished);
}


inline void Reply_p::AbortInlineRaw(){
    DisconnectAllConnectionsAndRetIfDisconnectedInline();
    if(m_flagsBS.rd.abortCalled_false){
        m_flagsBS.wr.abortCalled = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
        m_pQtNetReply->abort();
    }
}


inline void Reply_p::AbortInline2(){
    if(m_pQtNetReply){
        AbortInlineRaw();
    }
}


inline void AccessManager_p::DestroyQtNetAccessManagerInline(){
    Reply_p *pNextReply, *pReply = m_pFirst;
    while(pReply){
        pNextReply = pReply->m_next;
        pReply->AbortInline2();
        delete pReply;
        pReply = pNextReply;
    }  //  while(pReply){
    m_pFirst = nullptr;
    delete m_pQtNetAccessManager;
    m_pQtNetAccessManager = nullptr;
}


/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

AccessManager::~AccessManager()
{
    delete m_data_p;
}


AccessManager::AccessManager()
:
    m_data_p(new AccessManager_p())
{
}


Reply* AccessManager::AnyRestCall(int a_timeoutMs, const TypeRestCall& a_restCallFnc)
{
    QNetworkReply* const pQtNetReply = a_restCallFnc(m_data_p->m_pQtNetAccessManager);
    if(!pQtNetReply){
        throw ::std::runtime_error("Unable to create Qt QNetworkReply");
    }
    Reply* const pRetReply = new Reply(a_timeoutMs);
    if(m_data_p->m_pFirst){
        m_data_p->m_pFirst->m_prev = pRetReply->m_data_p;
    }
    m_data_p->m_pFirst = pRetReply->m_data_p;
    pRetReply->m_data_p->m_pParentAccessMngr = m_data_p;
    pRetReply->m_data_p->m_pQtNetReply = pQtNetReply;
    pRetReply->m_data_p->ConnectSignalsAndStartTimer();
    return pRetReply;
}


Reply* AccessManager::AnyRestCall(const TypeRestCall& a_restCallFnc)
{
    return AnyRestCall(-1,a_restCallFnc);
}


void AccessManager::RestartNetAccessManaget()
{
    m_data_p->DestroyQtNetAccessManagerInline();
    m_data_p->m_pQtNetAccessManager = new QNetworkAccessManager();
}


QNetworkAccessManager* AccessManager::pQtNetAccessMngr()const noexcept
{
    return m_data_p->m_pQtNetAccessManager;
}


/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

Reply::~Reply()
{
    delete m_data_p;
}


Reply::Reply(int a_timeoutMs)
:
    m_data_p(new Reply_p(this,a_timeoutMs))
{
}


void Reply::Abort()
{
    m_data_p->AbortInline2();
}


QNetworkReply* Reply::qtNetworkReply()const noexcept
{
    return m_data_p->m_pQtNetReply;
}


int Reply::timeoutMs()const noexcept
{
    return m_data_p->m_timeoutMs;
}


bool Reply::isTimedOut()const noexcept
{
    return static_cast<bool>(m_data_p->m_flagsBS.rd.hasTimeout_true);
}


/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

AccessManager_p::~AccessManager_p()
{
    DestroyQtNetAccessManagerInline();
}


AccessManager_p::AccessManager_p()
:
    m_pQtNetAccessManager(nullptr),
    m_pFirst(nullptr)
{
    qRegisterMetaType< QuCoreNetReplyArgV02 >( "QuCoreNetReplyArgV02" );
    m_pQtNetAccessManager = new QNetworkAccessManager();
}


/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

Reply_p::~Reply_p()
{
    if(m_pParentAccessMngr){
        if(this==(m_pParentAccessMngr->m_pFirst)){
            m_pParentAccessMngr->m_pFirst = this->m_next;
        }
        else{
            this->m_prev->m_next = m_next;
        }
        if(m_next){
            m_next->m_prev = m_prev;
        }
    }  //  if(m_pParentAccessMngr){
    if(m_pQtNetReply){
        AbortInlineRaw();
        delete m_pQtNetReply;
    }
}


Reply_p::Reply_p(Reply* CPPUTILS_ARG_NN a_pParent, int a_timeoutMs)
    :
    m_pParent(a_pParent),
    m_finishArg(a_pParent),
    m_pParentAccessMngr(nullptr),
    m_pQtNetReply(nullptr),
    m_timeoutMs(a_timeoutMs)
{
    m_prev = m_next = nullptr;
    m_flagsBS.wr_all = CPPUTILS_BISTATE_MAKE_ALL_BITS_FALSE;

}


void Reply_p::ConnectSignalsAndStartTimer()
{
    m_connDestroy = QObject::connect(m_pQtNetReply,&QObject::destroyed,m_pParent,[this](){
        const RaiiDeleter aDltr([this](){
            m_finishArg.reset();
        });
        m_pQtNetReply = nullptr;
        DisconnectAllConnectionsAndRetIfDisconnectedInline();
    });

    m_connFinished = ::QObject::connect(m_pQtNetReply,&QNetworkReply::finished,m_pParent,[this](){
        const RaiiDeleter aDltr([this](){
            m_finishArg.reset();
        });
        if(m_timeoutTimer.isActive()){
            DisconnectMetaConnectionInline(&m_connTimeout);
            m_timeoutTimer.stop();
        }
        DisconnectAllConnectionsAndRetIfDisconnectedInline();
        if(m_flagsBS.rd.finishEmitted_false){
            m_flagsBS.wr.finishEmitted = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
            emit m_pParent->finished(m_finishArg);
        }
    });

    if(m_timeoutMs>=0){
        m_connTimeout = QObject::connect(&(m_timeoutTimer),&QTimer::timeout,m_pParent,[this](){
            const RaiiDeleter aDltr([this](){
                m_finishArg.reset();
            });
            m_flagsBS.wr.hasTimeout = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
            DisconnectAllConnectionsAndRetIfDisconnectedInline();
            AbortInline2();
            if(m_flagsBS.rd.finishEmitted_false){
                m_flagsBS.wr.finishEmitted = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
                emit m_pParent->finished(m_finishArg);
            }
        });
        m_timeoutTimer.start(m_timeoutMs);
    }  //  if(m_timeout>=0){
}


/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

RaiiDeleter::~RaiiDeleter()
{
    m_dltr();
}


RaiiDeleter::RaiiDeleter(const TypeDeleter& a_dltr)
:
    m_dltr(a_dltr)
{
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


QTUTILS_EXPORT void ErrorByteArray(const QNetworkReply::NetworkError&,const Reply& a_replyHandlerIn, QByteArray* CPPUTILS_IN_OUT a_pData)
{
    QByteArray& responseByteArray = *a_pData;
    if(responseByteArray.isEmpty()){
        if(a_replyHandlerIn.isTimedOut()){
            responseByteArray = "timeout";
        }
        else{
            responseByteArray = a_replyHandlerIn.qtNetworkReply()->errorString().toLocal8Bit();
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



}}}  //  namespace qtutils { namespace core{ namespace network{
