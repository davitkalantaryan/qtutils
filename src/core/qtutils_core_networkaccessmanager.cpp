//
// file:            qtutils_networkaccessmanager.cpp
// path:			src/core/qtutils_networkaccessmanager.cpp
// created on:		2021 Nov 27
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/core/networkaccessmanager.hpp>
#include <qtutils/core/invokeblocked.hpp>
#include <qtutils/core/utils.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QThread>


namespace qtutils { namespace network {


Exception::Exception(network::ReplyData* a_pReplyData,const char* a_cpcWhat)
	:
	  ::std::runtime_error(a_cpcWhat),
	  m_pReplyData(a_pReplyData)
{
}

/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/


AccessManagerRaw::AccessManagerRaw()
{
    m_pQtManager = new QNetworkAccessManager();
}


AccessManagerRaw::~AccessManagerRaw()
{
    delete m_pQtManager;
}


Reply* AccessManagerRaw::post(ReplyContainer* a_pContainer, const QNetworkRequest& a_request, const QByteArray& a_data,  ReplyData* a_pData, int a_timeoutMs)
{
    QNetworkReply* pNetworkReply = m_pQtManager->post(a_request,a_data);
    if(pNetworkReply){
		return new Reply(pNetworkReply,a_pContainer,a_pData, a_timeoutMs);
    }
    //return nullptr;
	throw Exception(a_pData,"Unable to create Network Reply object");
}


Reply* AccessManagerRaw::post(ReplyContainer* a_pContainer, const QNetworkRequest& a_request, const QVariantMap& a_data,  ReplyData* a_pData, int a_timeoutMs)
{
    const QJsonDocument dataJsonDoc = QJsonDocument(QJsonObject::fromVariantMap(a_data));
    const QByteArray dataBA = dataJsonDoc.toJson(QJsonDocument::Compact);
    return post(a_pContainer,a_request,dataBA,a_pData,a_timeoutMs);
}


Reply* AccessManagerRaw::post(ReplyContainer* a_pContainer, const QNetworkRequest& a_request, QHttpMultiPart* a_pMultiPart,  ReplyData* a_pData, int a_timeoutMs)
{
    QNetworkReply* pNetworkReply = m_pQtManager->post(a_request,a_pMultiPart);
    if(pNetworkReply){
        return new Reply(pNetworkReply,a_pContainer,a_pData, a_timeoutMs);
    }
    //return nullptr;
    throw Exception(a_pData,"Unable to create Network Reply object");
}


Reply* AccessManagerRaw::get(ReplyContainer* a_pContainer, const QNetworkRequest& a_request, ReplyData* a_pData, int a_timeoutMs)
{
    QNetworkReply* pNetworkReply = m_pQtManager->get(a_request);
    if(pNetworkReply){
		return new Reply(pNetworkReply,a_pContainer,a_pData, a_timeoutMs);
    }
    //return nullptr;
	throw Exception(a_pData,"Unable to create Network Reply object");
}


Reply* AccessManagerRaw::head(ReplyContainer* a_pContainer, const QNetworkRequest& a_request, ReplyData* a_pData, int a_timeoutMs)
{
    QNetworkReply* pNetworkReply = m_pQtManager->head(a_request);
    if(pNetworkReply){
        return new Reply(pNetworkReply,a_pContainer,a_pData, a_timeoutMs);
    }
    //return nullptr;
    throw Exception(a_pData,"Unable to create Network Reply object");
}


Reply* AccessManagerRaw::deleteResource(ReplyContainer* a_pContainer, const QNetworkRequest& a_request, ReplyData* a_pData, int a_timeoutMs)
{
    QNetworkReply* pNetworkReply = m_pQtManager->deleteResource(a_request);
    if(pNetworkReply){
		return new Reply(pNetworkReply,a_pContainer,a_pData, a_timeoutMs);
    }
    //return nullptr;
	throw Exception(a_pData,"Unable to create Network Reply object");
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

AccessManager::AccessManager()
{
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
        delete pReply;
        pReply = pReplyTmp;
    }
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

ReplyData::~ReplyData()
{
}


QByteArray ReplyData::postData()const
{
    return QByteArray();
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

Reply::Reply( QNetworkReply* CPPUTILS_NO_NULL a_networkReply, ReplyContainer* a_pParentContainer, ReplyData* a_pData, int a_timeoutTimer)
    :
      m_pNetworkReply(a_networkReply),
      m_pParentContainer(a_pParentContainer),
      m_pData(a_pData),
      m_restStartDate(QDateTime::currentDateTime())
{
    m_bHasTimeout = false;
    
    if(a_pParentContainer){
        a_pParentContainer->AddNewNetworkReply(this);
    }
    else{
        m_prev=(nullptr);
        m_next=(nullptr);
    }

	m_connDestroy = connect(m_pNetworkReply,&QObject::destroyed,this,[this](){
		m_pNetworkReply = nullptr;
		deleteLater();
	});

    m_connFinished = ::QObject::connect(m_pNetworkReply,&QNetworkReply::finished,this,[this](){
        if(m_timeoutTimer.isActive()){
            m_timeoutTimer.stop();
        }
        emit finished();
    });
    
    if(a_timeoutTimer>=0){
        QObject::connect(&m_timeoutTimer,&QTimer::timeout,this,[this](){
            m_bHasTimeout = true;
            Abort();
            emit finished();
        });
        m_timeoutTimer.start(a_timeoutTimer);
    }
}


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


}}  // namespace qtutils { namespace network {
