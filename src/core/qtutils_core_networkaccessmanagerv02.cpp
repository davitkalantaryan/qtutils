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
#include <QEvent>
#include <QEventLoop>
#include <QCoreApplication>
#include <cinternal/undisable_compiler_warnings.h>


namespace qtutils { namespace core{ namespace network{


class CPPUTILS_DLL_PRIVATE QuCoreNetReplyArgV02_p
{
public:
    bool Reset() noexcept;
private:
    ::qtutils::core::network::Reply*    m_pReply;
    int                                 m_count;
    friend class ::QuCoreNetReplyArgV02;
};


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


class CPPUTILS_DLL_PRIVATE NetStopEvent final : public QEvent
{
public:
    NetStopEvent();
private:
    NetStopEvent(const NetStopEvent&)=delete;
    NetStopEvent(NetStopEvent&&)=delete;
    NetStopEvent& operator=(const NetStopEvent&)=delete;
    NetStopEvent& operator=(NetStopEvent&&)=delete;
};


class CPPUTILS_DLL_PRIVATE NetWaitEventLoop final : public QEventLoop
{
public:
    NetWaitEventLoop(AccessManager_p* CPPUTILS_ARG_NN a_netAccMngr_p);

private:
    bool event(QEvent* a_event) override;

private:
    AccessManager_p* const      m_netAccMngr_p;
};


class CPPUTILS_DLL_PRIVATE Reply_p
{
public:
    Reply* const                    m_pParent;
    QuCoreNetReplyArgV02            m_finishArg;
    AccessManager_p* const          m_pParentAccessMngr;
    QNetworkReply*                  m_pQtNetReply;
    const int                       m_timeoutMs;
    QMetaObject::Connection         m_connFinished;
    QMetaObject::Connection         m_connTimeout;
    QMetaObject::Connection         m_connDestroy;
    QTimer                          m_timeoutTimer;
    Reply_p                         *m_prev, *m_next;
    CPPUTILS_BISTATE_FLAGS_UN(
        hasTimeout,
        abortOrFinishCalled,
        finishEmitted,
        blockExit,
        hasRealFinish
    )m_flagsBS;

public:
    ~Reply_p();
    Reply_p(Reply* CPPUTILS_ARG_NN a_pParent,AccessManager_p* CPPUTILS_ARG_NN a_pAccsMngr, QNetworkReply* CPPUTILS_ARG_NN a_pQtNetReply, int a_timeoutMs);

    inline void DisconnectAllConnectionsInline();
    inline void EmitFinishedInline();
    inline void AbortInline();
    inline void ResetInline() noexcept;

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
    NetWaitEventLoop        m_finalLoop;
    QTimer                  m_tmrEL;
    Reply_p*                m_pFirst;
    Reply_p*                m_pLast;
    int                     m_exitTimeoutMs;
    CPPUTILS_BISTATE_FLAGS_UN(
        quitOngoing
    )m_flagsBS;

public:
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


inline void Reply_p::DisconnectAllConnectionsInline(){
    DisconnectMetaConnectionInline(&m_connDestroy);
    DisconnectMetaConnectionInline(&m_connTimeout);
    DisconnectMetaConnectionInline(&m_connFinished);
}


inline void Reply_p::EmitFinishedInline(){
    if(m_flagsBS.rd.finishEmitted_false){
        m_flagsBS.wr.finishEmitted = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
        emit m_pParent->finished(m_finishArg);
    }
}


inline void Reply_p::AbortInline(){
    if((m_flagsBS.rd.abortOrFinishCalled_false)&&m_pQtNetReply){
        m_flagsBS.wr.abortOrFinishCalled = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
        m_pQtNetReply->abort();
    }
}


inline void Reply_p::ResetInline() noexcept
{
    QuCoreNetReplyArgV02_p* const pShrdPtr = m_finishArg.m_data_p;
    if(pShrdPtr){
        pShrdPtr->Reset();
    }
}


inline void AccessManager_p::DestroyQtNetAccessManagerInline()
{
    if(m_pFirst){
        m_flagsBS.wr.quitOngoing = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
        Reply_p *pNextReply, *pReply = m_pFirst;
        while(pReply){
            pNextReply = pReply->m_next;
            if(pReply->m_flagsBS.rd.blockExit_false){
                pReply->DisconnectAllConnectionsInline();
                pReply->AbortInline();
                pReply->EmitFinishedInline();
                pReply->ResetInline();
            }  //  if(pReply->m_flagsBS.rd.blockExit_false){
            pReply = pNextReply;
        }  //  while(pReply){
        if(m_exitTimeoutMs>=0){
            QObject::connect(&m_tmrEL, &QTimer::timeout, &m_finalLoop,[this](){
                m_tmrEL.stop();
                Reply_p *pNextReply, *pReply = m_pFirst;
                while(pReply){
                    pNextReply = pReply->m_next;
                    pReply->DisconnectAllConnectionsInline();
                    pReply->AbortInline();
                    pReply->EmitFinishedInline();
                    pReply->ResetInline();
                    pReply = pNextReply;
                }  //  while(pReply){
            });  //  QObject::connect(&tmrEL, &QTimer::timeout, &loop,[this,&loop](){
            m_tmrEL.setSingleShot(true);
            m_tmrEL.start(m_exitTimeoutMs);
        }  //  if(a_timeoutMs>=0){
        m_finalLoop.exec();
        m_flagsBS.wr.quitOngoing = CPPUTILS_BISTATE_MAKE_BITS_FALSE;
    }  //  if(m_pFirst){

    m_pLast = m_pFirst = nullptr;
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
    Reply* const pRetReply = new Reply(m_data_p,pQtNetReply,a_timeoutMs);
    return pRetReply;
}


Reply* AccessManager::AnyRestCall(const TypeRestCall& a_restCallFnc)
{
    return AnyRestCall(-1,a_restCallFnc);
}


void AccessManager::RestartNetAccessManager()
{
    m_data_p->DestroyQtNetAccessManagerInline();
    m_data_p->m_pQtNetAccessManager = new QNetworkAccessManager();
}


void AccessManager::StopAndCleanNetAccessManager()
{
    m_data_p->DestroyQtNetAccessManagerInline();
}


QNetworkAccessManager* AccessManager::pQtNetAccessMngr()const noexcept
{
    return m_data_p->m_pQtNetAccessManager;
}


int AccessManager::exitTimeoutMs()const noexcept
{
    return m_data_p->m_exitTimeoutMs;
}


void AccessManager::SetExitTimeoutMs(int a_exitTimeoutMs)noexcept
{
    m_data_p->m_exitTimeoutMs = a_exitTimeoutMs;
}


/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

Reply::~Reply()
{
    delete m_data_p;
}


Reply::Reply(AccessManager_p* CPPUTILS_ARG_NN a_pAccsMngr, QNetworkReply* CPPUTILS_ARG_NN a_pQtNetReply, int a_timeoutMs)
:
    m_data_p(new Reply_p(this,a_pAccsMngr,a_pQtNetReply,a_timeoutMs))
{
}


void Reply::Abort()
{
    m_data_p->AbortInline();
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


void Reply::MakeThisCallBlockExit()noexcept
{
    m_data_p->m_flagsBS.wr.blockExit = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
}


/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

AccessManager_p::AccessManager_p()
:
    m_pQtNetAccessManager(nullptr),
    m_finalLoop(this),
    m_pFirst(nullptr),
    m_pLast(nullptr),
    m_exitTimeoutMs(-1)
{
    m_flagsBS.wr_all = CPPUTILS_BISTATE_MAKE_ALL_BITS_FALSE;
    qRegisterMetaType< QuCoreNetReplyArgV02 >( "QuCoreNetReplyArgV02" );
    m_pQtNetAccessManager = new QNetworkAccessManager();
}


/*///////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

Reply_p::~Reply_p()
{
    DisconnectAllConnectionsInline();
    QuCoreNetReplyArgV02_p* const pShrdMem = m_finishArg.m_data_p;
    m_finishArg.m_data_p = nullptr;
    delete pShrdMem;
    if(m_prev){
        m_prev->m_next = this->m_next;
    }
    else{
        // if tehre is no previous, then this one is teh first
        m_pParentAccessMngr->m_pFirst = m_next;
    }
    if(m_next){
        m_next->m_prev = this->m_prev;
    }
    else{
        // this is last one
        m_pParentAccessMngr->m_pLast = this->m_prev;
    }
    delete m_pQtNetReply;
    if(m_pParentAccessMngr->m_flagsBS.rd.quitOngoing_true){
        NetStopEvent* const pStpEvnt = new NetStopEvent();
        QCoreApplication::postEvent(&(m_pParentAccessMngr->m_finalLoop),pStpEvnt);
    }
}


Reply_p::Reply_p(Reply* CPPUTILS_ARG_NN a_pParent,AccessManager_p* CPPUTILS_ARG_NN a_pAccsMngr, QNetworkReply* CPPUTILS_ARG_NN a_pQtNetReply, int a_timeoutMs)
    :
    m_pParent(a_pParent),
    m_finishArg(a_pParent),
    m_pParentAccessMngr(a_pAccsMngr),
    m_pQtNetReply(a_pQtNetReply),
    m_timeoutMs(a_timeoutMs)
{
    m_flagsBS.wr_all = CPPUTILS_BISTATE_MAKE_ALL_BITS_FALSE;
    m_next = nullptr;
    if(m_pParentAccessMngr->m_pLast){
        m_pParentAccessMngr->m_pLast->m_next = this;
    }
    else {
        m_pParentAccessMngr->m_pFirst = this;
    }
    m_prev = m_pParentAccessMngr->m_pLast;
    m_pParentAccessMngr->m_pLast = this;

    m_connDestroy = QObject::connect(m_pQtNetReply,&QObject::destroyed,m_pParent,[this](){
        const RaiiDeleter aDltr([this](){
            ResetInline();
        });
        m_pQtNetReply = nullptr;
        m_flagsBS.wr.blockExit = CPPUTILS_BISTATE_MAKE_BITS_FALSE;
        DisconnectAllConnectionsInline();
        if(m_timeoutTimer.isActive()){
            m_timeoutTimer.stop();
        }
        EmitFinishedInline();
    });

    m_connFinished = ::QObject::connect(m_pQtNetReply,&QNetworkReply::finished,m_pParent,[this](){
        const RaiiDeleter aDltr([this](){
            ResetInline();
        });
        m_flagsBS.wr.abortOrFinishCalled = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
        m_flagsBS.wr.blockExit = CPPUTILS_BISTATE_MAKE_BITS_FALSE;
        m_flagsBS.wr.hasRealFinish = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
        DisconnectAllConnectionsInline();
        if(m_timeoutTimer.isActive()){
            m_timeoutTimer.stop();
        }
        EmitFinishedInline();
    });

    if(m_timeoutMs>=0){
        m_connTimeout = QObject::connect(&(m_timeoutTimer),&QTimer::timeout,m_pParent,[this](){
            const RaiiDeleter aDltr([this](){
                ResetInline();
            });
            m_flagsBS.wr.blockExit = CPPUTILS_BISTATE_MAKE_BITS_FALSE;
            m_flagsBS.wr.hasTimeout = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
            DisconnectAllConnectionsInline();
            AbortInline();
            EmitFinishedInline();
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

static inline QEvent::Type GenerateAndGetEventType02Inline(){
    static const QEvent::Type evType = static_cast<QEvent::Type>(QEvent::registerEventType());
    return evType;
}


NetStopEvent::NetStopEvent()
    :
    QEvent(GenerateAndGetEventType02Inline())
{
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

NetWaitEventLoop::NetWaitEventLoop(AccessManager_p* CPPUTILS_ARG_NN a_netAccMngr_p)
    :
    m_netAccMngr_p(a_netAccMngr_p)
{
}


bool NetWaitEventLoop::event(QEvent* a_event)
{
    if(a_event->type()==GenerateAndGetEventType02Inline()){
        if(!(m_netAccMngr_p->m_pFirst)){
            if(m_netAccMngr_p->m_tmrEL.isActive()){
                m_netAccMngr_p->m_tmrEL.stop();
            }
            if(this->isRunning()){
                this->quit();
            }
        }  //  if(!(m_netAccMngr_p->m_pFirst)){
        return true;
    }  //  f(a_event->type()==GenerateAndGetEventTypeInline()){
    return QEventLoop::event(a_event);
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


bool QuCoreNetReplyArgV02_p::Reset() noexcept
{
    if((--(m_count))<1){
        if(m_pReply){
            ::qtutils::core::network::Reply* const pReply = m_pReply;
            m_pReply = nullptr;
            pReply->deleteLater();
            return true;
        }  //  if(m_pReply){
    }  //  if((--(m_count))<1){
    return false;
}


}}}  //  namespace qtutils { namespace core{ namespace network{


QuCoreNetReplyArgV02::~QuCoreNetReplyArgV02() noexcept
{
    if(m_data_p){
        if(m_data_p->Reset()){
            delete m_data_p;
        }
    }  //  if(m_data_p){
}


QuCoreNetReplyArgV02::QuCoreNetReplyArgV02(::qtutils::core::network::Reply* CPPUTILS_ARG_NN a_pReply)
    :
    m_data_p(nullptr)
{
    m_data_p = new ::qtutils::core::network::QuCoreNetReplyArgV02_p();
    m_data_p->m_count = 1;
    m_data_p->m_pReply = a_pReply;
}


QuCoreNetReplyArgV02::QuCoreNetReplyArgV02(const QuCoreNetReplyArgV02& a_cM) noexcept
    :
    m_data_p(a_cM.m_data_p)
{
    if(m_data_p){
        ++(m_data_p->m_count);
    }
}


QuCoreNetReplyArgV02::QuCoreNetReplyArgV02(QuCoreNetReplyArgV02&& a_mM) noexcept
    :
    m_data_p(a_mM.m_data_p)
{
    a_mM.m_data_p = nullptr;
}


QuCoreNetReplyArgV02& QuCoreNetReplyArgV02::operator=(const QuCoreNetReplyArgV02& a_cM) noexcept
{
    if(m_data_p!=(a_cM.m_data_p)){
        if(m_data_p){
            if(m_data_p->Reset()){
                delete m_data_p;
            }  //  if(m_data_p->Reset()){
        }  //  if(m_data_p){

        m_data_p = a_cM.m_data_p;
        if(m_data_p){
            ++(m_data_p->m_count);
        }
    }  //  if(m_data_p!=(a_cM.m_data_p)){

    return *this;
}


QuCoreNetReplyArgV02& QuCoreNetReplyArgV02::operator=(QuCoreNetReplyArgV02&& a_mM) noexcept
{
    ::qtutils::core::network::QuCoreNetReplyArgV02_p* const pThis = m_data_p;
    m_data_p = a_mM.m_data_p;
    a_mM.m_data_p = pThis;
    return *this;
}


::qtutils::core::network::Reply* QuCoreNetReplyArgV02::get()const noexcept
{
    if(m_data_p){
        return m_data_p->m_pReply;
    }
    return nullptr;
}
