//
// file:            networkaccessmanager.hpp
// path:			src/include/private/monitor/core/networkaccessmanager.hpp
// created on:		2021 Nov 27
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#include <focust/core/focust_internal_header.h>
#include <stdexcept>
#include <focust/core/disable_utils_warnings.h>
#include <QNetworkAccessManager>
#include <QNetworkReply>
//#include <QHttpMultiPart>

namespace focust { namespace network {


class FOCUST_DLL_PRIVATE Reply;
class FOCUST_DLL_PRIVATE ReplyContainer;
class FOCUST_DLL_PRIVATE ReplyData;
class FOCUST_DLL_PRIVATE AccessManager;

class FOCUST_DLL_PRIVATE Exception final : public ::std::runtime_error
{
public:
	Exception(network::ReplyData* a_pReplyData,const char* a_cpcWhat);
public:
	network::ReplyData*const m_pReplyData;
};


class FOCUST_DLL_PRIVATE AccessManagerRaw final
{
private:
    AccessManagerRaw();
    ~AccessManagerRaw();
    
public:
    Reply* post(ReplyContainer* a_pContainer, const QNetworkRequest &request, const QByteArray &data,  ReplyData* a_pData);
    Reply* get(ReplyContainer* a_pContainer, const QNetworkRequest &request, ReplyData* a_pData);
    //Reply* post(ReplyContainer* a_pContainer, const Request &request, HttpMultiPart *multiPart,ReplyData* a_pData);
    //Reply* deleteResource(ReplyContainer* a_pContainer, const Request &request, ReplyData* a_pData);
    //Reply* get(ReplyContainer* a_pContainer,const Request &request, ReplyData* a_pData);
	// new API
	//Reply* postBA(NetworkRequestCarier* a_carier);
	//Reply* postMPart(NetworkRequestCarier* a_carier);
	//Reply* get(NetworkRequestCarier* a_carier);
    
private:
    QNetworkAccessManager*    m_pQtManager;
    
    friend class AccessManager;
};


class FOCUST_DLL_PRIVATE AccessManager final
{
public:
    AccessManager();
    AccessManagerRaw*   accessManagerRaw();
    void   Restart();
    
private:
    AccessManagerRaw	m_rawManager;
    bool				m_bPendingRestart;
};


class FOCUST_DLL_PRIVATE ReplyContainer final
{
public:
    ReplyContainer();
    ReplyContainer(const ReplyContainer&)=delete;
    ReplyContainer(ReplyContainer&&)=delete;
    ~ReplyContainer();
    
    void AddNewNetworkReply(Reply* a_pReply);
    void RemoveNetworkReply(Reply* a_pReply);
    
protected:
    Reply    *m_pFirst, *m_pLast;    
};


class FOCUST_DLL_PRIVATE ReplyData
{
public:
    virtual ~ReplyData();    
};


class FOCUST_DLL_PRIVATE Reply final : public QObject
{
    Q_OBJECT

private:
    Reply() = delete;
    Reply(const Reply&) = delete;
    Reply(Reply&&) = delete;
    Reply( QNetworkReply* FOCUST_NO_NULL networkReply, ReplyContainer* a_pParentContainer, ReplyData* a_pData=nullptr);

public:
    ~Reply() override;
    
    void Abort();
    QNetworkReply* operator->()const;
    ReplyData* data()const;
    void ReplaceData(ReplyData* a_pData); // this will be used to replace by null to prevent delete
    
private:
signals:
    void finished();

private:
    Reply                   *m_prev, *m_next;
    QNetworkReply*/*const*/	m_pNetworkReply;
    ReplyContainer*/*const*/m_pParentContainer;
    ReplyData*              m_pData;
    QMetaObject::Connection	m_connFinished;
    QMetaObject::Connection	m_connDestroy;
    
    friend class ReplyContainer;
    friend class AccessManagerRaw;
};


}}  // namespace focust { namespace network {
