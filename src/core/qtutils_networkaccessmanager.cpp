//
// file:            focust_networkaccessmanager.cpp
// path:			src/core/focust_networkaccessmanager.cpp
// created on:		2021 Nov 27
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <focust/core/networkaccessmanager.hpp>


namespace focust { namespace network {


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


Reply* AccessManagerRaw::post(ReplyContainer* a_pContainer, const QNetworkRequest& a_request, const QByteArray& a_data,  ReplyData* a_pData)
{
    QNetworkReply* pNetworkReply = m_pQtManager->post(a_request,a_data);
    if(pNetworkReply){
		return new Reply(pNetworkReply,a_pContainer,a_pData);
    }
    //return nullptr;
	throw Exception(a_pData,"Unable to create Network Reply object");
}


Reply* AccessManagerRaw::get(ReplyContainer* a_pContainer, const QNetworkRequest& a_request, ReplyData* a_pData)
{
    QNetworkReply* pNetworkReply = m_pQtManager->get(a_request);
    if(pNetworkReply){
		return new Reply(pNetworkReply,a_pContainer,a_pData);
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
    Reply *pReplyTmp, *pReply = m_pFirst;
    while(pReply){
        pReply->m_pParentContainer = nullptr;
        pReplyTmp = pReply->m_next;
        pReply->Abort();
        delete pReply;
        pReply = pReplyTmp;
    }
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


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

ReplyData::~ReplyData()
{
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

Reply::Reply( QNetworkReply* FOCUST_NO_NULL a_networkReply, ReplyContainer* a_pParentContainer, ReplyData* a_pData)
    :
      m_pNetworkReply(a_networkReply),
      m_pParentContainer(a_pParentContainer),
      m_pData(a_pData)
{
    if(a_pParentContainer){
        a_pParentContainer->AddNewNetworkReply(this);;
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
        emit finished();
    });
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


}}  // namespace focust { namespace network {
