//
// file:            networkaccessmanager.hpp
// path:			src/include/private/monitor/core/networkaccessmanager.hpp
// created on:		2021 Nov 27
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#include <qtutils/qtutils_internal_header.h>
#include <stdexcept>
#include <qtutils/disable_utils_warnings.h>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QVariantMap>
#include <QDateTime>
#include <QByteArray>

namespace qtutils { namespace network{


class CPPUTILS_DLL_PRIVATE Reply;
class CPPUTILS_DLL_PRIVATE ReplyContainer;
class CPPUTILS_DLL_PRIVATE ReplyData;
class CPPUTILS_DLL_PRIVATE AccessManager;

class CPPUTILS_DLL_PRIVATE Exception final : public ::std::runtime_error
{
public:
	Exception(ReplyData* a_pReplyData,const char* a_cpcWhat);
public:
	ReplyData*const m_pReplyData;
};


class QTUTILS_EXPORT AccessManagerRaw final
{
private:
    AccessManagerRaw();
    ~AccessManagerRaw();
    
public:
    Reply* post(ReplyContainer* a_pContainer, const QNetworkRequest &request, const QByteArray &data,  ReplyData* a_pData, int a_timeoutMs);
    Reply* post(ReplyContainer* a_pContainer, const QNetworkRequest &request, const QVariantMap&data,  ReplyData* a_pData, int a_timeoutMs);
    Reply* post(ReplyContainer* a_pContainer, const QNetworkRequest &request, QHttpMultiPart* multiPart,  ReplyData* a_pData, int a_timeoutMs);
    Reply* get(ReplyContainer* a_pContainer, const QNetworkRequest &request, ReplyData* a_pData, int a_timeoutMs);
    Reply* deleteResource(ReplyContainer* a_pContainer, const QNetworkRequest &request, ReplyData* a_pData, int a_timeoutMs);
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


class QTUTILS_EXPORT AccessManager final
{
public:
    AccessManager();
    AccessManagerRaw*   accessManagerRaw();
    void   Restart();
    
private:
    AccessManagerRaw	m_rawManager;
    bool				m_bPendingRestart;
};


class QTUTILS_EXPORT ReplyContainer final
{
public:
    ReplyContainer();
    ReplyContainer(const ReplyContainer&)=delete;
    ReplyContainer(ReplyContainer&&)=delete;
    ~ReplyContainer();
    
    void AddNewNetworkReply(Reply* a_pReply);
    void RemoveNetworkReply(Reply* a_pReply);

private:
    void Clear();
    
protected:
    Reply    *m_pFirst, *m_pLast;    
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

private:
    Reply() = delete;
    Reply(const Reply&) = delete;
    Reply(Reply&&) = delete;
    Reply( QNetworkReply* CPPUTILS_NO_NULL networkReply, ReplyContainer* a_pParentContainer, ReplyData* a_pData=nullptr, int a_timeoutMs=-1);

public:
    ~Reply() override;
    
    void Abort();
    QNetworkReply* operator->()const;
    QNetworkReply* qtNetworkReply()const;
    ReplyData* data()const;
    void ReplaceData(ReplyData* a_pData); // this will be used to replace by null to prevent delete
    bool hasTimeout()const;
    
private:
signals:
    void finished();

private:
    Reply                   *m_prev, *m_next;
    QTimer                  m_timeoutTimer;
    QNetworkReply*/*const*/	m_pNetworkReply;
    ReplyContainer*/*const*/m_pParentContainer;
    ReplyData*              m_pData;
    QMetaObject::Connection	m_connFinished;
    QMetaObject::Connection	m_connDestroy;
    bool                    m_bHasTimeout;

public:
    const QDateTime         m_restStartDate;
    
    friend class ReplyContainer;
    friend class AccessManagerRaw;
};


QTUTILS_EXPORT void PrepareJsonHeaders(QNetworkRequest* a_pRequet, const QString& a_agent);
QTUTILS_EXPORT void PrepareJsonHeadersWithAuth(QNetworkRequest* a_pRequet, const QString& a_authToken, const QString& a_agent);
QTUTILS_EXPORT void PrepareMPartHeadersWithAuth(QNetworkRequest* a_pRequet, const QString& a_authToken,const QString& a_agent);
QTUTILS_EXPORT void ErrorByteArray(const QNetworkReply::NetworkError&,const ::qtutils::network::Reply& a_replyHandlerIn, QByteArray* CPPUTILS_IN_OUT a_pData);
QTUTILS_EXPORT QString CorectUrl(const QString& a_url);
QTUTILS_EXPORT QString NetworkErrorCodeString(const QNetworkReply::NetworkError& a_errorCode);


}}  // namespace qtutils { namespace network{
