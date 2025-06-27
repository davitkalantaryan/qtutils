//
// file:			kalantaryan_llfrontend_hosting_httpserver.cpp
// path:			${repositoryRoot}/src/core/kalantaryan_llfrontend_hosting/kalantaryan_llfrontend_hosting_httpserver.cpp
// created on:		2023 Jun 18
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/core/httpserver.hpp>
#include <qtutils/core/logger.hpp>
#include <cinternal/disable_compiler_warnings.h>
#include <qtutils/disable_utils_warnings.h>
#include <QHttpServerResponse>
#include <QUrl>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QSettings>
#include <QVariant>
#include <QVariantList>
#include <QVariantMap>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QSslServer>
#include <QThread>
#include <cinternal/undisable_compiler_warnings.h>


namespace qtutils { namespace core{


class CPPUTILS_DLL_PRIVATE HttpServer_p final
{
public:
    ~HttpServer_p();
    HttpServer_p();
    
public:
    HttpServer::TypeHashS       straightRoutes;
    HttpServer::TypeHashD       dirRoutes;
    HttpServer::TypeListRE      globRegExpRoutes;
    HttpServer::TypeListRE      wildcardRegExpRoutes;
    HttpServer::TypeListAA      anyAppearanceRoutes;
    HttpServer::TypeListAnM     anyMatcherRoutes;
    ByteArrayList               allowedHeaders;
    ByteArrayList               allowedOrigins;
    QTcpServer*                 m_pCurrentServer;
    QThread* const              m_pHttpThread;
};


template <typename BaseCls>
class Server : public BaseCls
{
public:
    ~Server() override {}
    Server(HttpServer_p* CPPUTILS_ARG_NN a_parent_p) :
        BaseCls(), m_parent_p(a_parent_p)
    {}
private:
    void incomingConnection(qintptr a_handle) override {
        m_parent_p->m_pCurrentServer = this;
        BaseCls::incomingConnection(a_handle);
    }

private:
    HttpServer_p* const     m_parent_p;

private:
    Server(const Server&)=delete;
    Server(Server&&)=delete;
    Server& operator=(const Server&)=delete;
    Server& operator=(Server&&)=delete;
};


/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

static inline void HttpServerFixResponceHeadersInline(bool a_bHasOrigin, const QByteArray& a_baOrigin, const ByteArrayList& a_allowedHeaders, QHttpServerResponse* CPPUTILS_ARG_NN a_pResp){
    QByteArray allowedHeaders = "content-type, authorization, client-device";
    const qsizetype headersCount = a_allowedHeaders.size();
    QByteArray nextHeader;

    for(qsizetype i(0); i<headersCount; ++i){
        nextHeader = a_allowedHeaders.at(i);
        allowedHeaders += ( QByteArray(", ") +  nextHeader);
    }  //  for(qsizetype i(0); i<headersCount; ++i){
        
#if QT_VERSION < QT_VERSION_CHECK(6, 8, 0)
    a_pResp->addHeader("Access-Control-Allow-Credentials","true");
    a_pResp->addHeader("Access-Control-Allow-Headers",allowedHeaders);
    a_pResp->addHeader("Access-Control-Allow-Methods","GET, OPTIONS, POST, PUT, PATCH, DELETE");
    a_pResp->addHeader("Access-Control-Max-Age","3600");
    if(a_bHasOrigin){
        a_pResp->addHeader("Access-Control-Allow-Origin",a_baOrigin);
    }
#else
    QHttpHeaders existingHeaders = a_pResp->headers();
    existingHeaders.append("Access-Control-Allow-Credentials","true");
    existingHeaders.append("Access-Control-Allow-Headers",allowedHeaders);
    existingHeaders.append("Access-Control-Allow-Methods","GET, OPTIONS, POST, PUT, PATCH, DELETE");
    existingHeaders.append("Access-Control-Max-Age","3600");
    if(a_bHasOrigin){
        existingHeaders.append("Access-Control-Allow-Origin",a_baOrigin);
    }
    a_pResp->setHeaders(existingHeaders);
#endif
}


static inline bool HttpServerCheckAndFixResponceHeadersInlineRaw(const TypeRestHeaders& a_vHeaders,
                                                                 const ByteArrayList& a_allowedHeaders, const ByteArrayList& a_allowedOrigins,
                                                                 QHttpServerResponse* CPPUTILS_ARG_NN a_pResp){
    bool bHasOrigin = false;
    qsizetype i;
    QByteArray baOrigin;
    const qsizetype cnHeadersCount = a_vHeaders.size();

    for(i = 0; i < cnHeadersCount; ++i){        
#if QT_VERSION < QT_VERSION_CHECK(6, 7, 0)
        if(a_vHeaders[i].first == QByteArray("Origin")){
            baOrigin = a_vHeaders[i].second;
            bHasOrigin = true;
            break;
        }  //  if(a_vHeaders[i].first == QByteArray("Origin")){
#else
        if(a_vHeaders.nameAt(i).compare("origin",Qt::CaseInsensitive)==0){
            const QByteArrayView baViewOrigin = a_vHeaders.valueAt(i);
            baOrigin = QByteArray(baViewOrigin.data(),baViewOrigin.size());
            bHasOrigin = true;
            break;
        }  //  if(a_vHeaders[i].first == QByteArray("Origin")){
#endif
    }  //  for(i = 0; i < cnHeadersCount; ++i){

    if(bHasOrigin){
        QByteArray tmpAllowedOrigin;
        const qsizetype allowedOriginsCount = a_allowedOrigins.size();
        for(i = 0; i < allowedOriginsCount; ++i){
            tmpAllowedOrigin = a_allowedOrigins.at(i);
            if(tmpAllowedOrigin==baOrigin){
                HttpServerFixResponceHeadersInline(true,baOrigin,a_allowedHeaders,a_pResp);
                return true;
            }  //  if(tmpAllowedOrigin==baOrigin){
            else {
                const QString tmpAllowedOriginStr = QString(tmpAllowedOrigin);
                const QRegularExpression reg(tmpAllowedOriginStr);
                const QRegularExpressionMatch matchg = reg.match(baOrigin);
                if(matchg.hasMatch()){
                    HttpServerFixResponceHeadersInline(true,baOrigin,a_allowedHeaders,a_pResp);
                    return true;
                }  //  if(matchg.hasMatch()){
                else{
                    const QRegularExpression rew = QRegularExpression::fromWildcard(tmpAllowedOriginStr,Qt::CaseSensitive,QRegularExpression::UnanchoredWildcardConversion);
                    const QRegularExpressionMatch matchw = rew.match(baOrigin);
                    if(matchw.hasMatch()){
                        HttpServerFixResponceHeadersInline(true,baOrigin,a_allowedHeaders,a_pResp);
                        return true;
                    }  //  if(matchw.hasMatch()){
                }  //  else of if(matchg.hasMatch()){
            }  //  else of if(tmpAllowedOrigin==baOrigin){
        }  //  for(i = 0; i < allowedOriginsCount; ++i){

        // origin provided, but it is not from list
        return false;
    }  //  if(bHasOrigin){

    HttpServerFixResponceHeadersInline(false,baOrigin,a_allowedHeaders,a_pResp);
    return true;
}


static inline bool HttpServerCheckAndFixResponceHeadersInline1(const QHttpServerRequest& a_request,
                                                               const ByteArrayList& a_allowedHeaders, const ByteArrayList& a_allowedOrigins,
                                                               QHttpServerResponse* CPPUTILS_ARG_NN a_pResp){
    return HttpServerCheckAndFixResponceHeadersInlineRaw(a_request.headers(),a_allowedHeaders,a_allowedOrigins,a_pResp);
}


static inline ByteArrayList VariantListToByteArrayListInline(const QVariantList& a_listVL){
    ByteArrayList retList;
    const qsizetype listSize = a_listVL.size();
    for(qsizetype i(0); i < listSize; ++i){
        retList.push_back(a_listVL.at(i).toByteArray());
    }  //  for(qsizetype i(0); i < allowedHeadersCount; ++i){
    return retList;
}


static inline ByteArrayList GetByteArrayListFromSettingsInline(const QString& a_key, const QSettings& a_settings){
    const QVariantList listVL = a_settings.value(a_key,QList<QVariant>()).toList();
    return VariantListToByteArrayListInline(listVL);
}


template <typename ContainerType>
static inline QVariantList  AnyDataFromAnyContainerToVariantListInline(const ContainerType& a_list){
    QVariantList listVL;
    const typename ContainerType::const_iterator iterEnd = a_list.cend();
    typename ContainerType::const_iterator iter = a_list.cbegin();
    for(;iter != iterEnd; ++iter){
        listVL.push_back(*iter);
    }
    return listVL;
}


template <typename ContainerType>
static inline QVariantList  AnyDataFromAnyMapContainerToVariantListInline(const ContainerType& a_list){
    QVariantList listVL;
    const typename ContainerType::const_iterator iterEnd = a_list.cend();
    typename ContainerType::const_iterator iter = a_list.cbegin();
    for(;iter != iterEnd; ++iter){
        listVL.push_back(iter->first);
    }
    return listVL;
}

typedef bool(*TypeAnyMatcherApiRaw)(const QUrl&, void*);

template <typename ContainerType>
static inline QVariantList  AnyDataFromAnyTupleContainerToVariantListInline(const ContainerType& a_list){
    QVariantList listVL;
    const void* pFunc;
    const typename ContainerType::const_iterator iterEnd = a_list.cend();
    typename ContainerType::const_iterator iter = a_list.cbegin();
    for(;iter != iterEnd; ++iter){
        pFunc = reinterpret_cast<const void*>(::std::get<0>(*iter).template target< TypeAnyMatcherApiRaw >());
        listVL.push_back( QString::asprintf("fa: %p",pFunc) );
    }
    return listVL;
}


static inline QVariantList ByteArrayListToVariantListInline(const ByteArrayList& a_list){
    return AnyDataFromAnyContainerToVariantListInline(a_list);
}


static inline void SetByteArrayListToSettingsInline(const QString& a_key, const ByteArrayList& a_list, QSettings* CPPUTILS_ARG_NN a_settings_p){
    const QList<QVariant> listVL = ByteArrayListToVariantListInline(a_list);
    a_settings_p->setValue(a_key,listVL);
}


/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

HttpServer::~HttpServer()
{
    delete m_server_data;
}


HttpServer::HttpServer()
    :
      m_server_data(new HttpServer_p())
{
    const QSettings aSettings;
    m_server_data->allowedHeaders = GetByteArrayListFromSettingsInline(QTUTILS_CORE_HTTPSERVER_ALLOWED_HEADERS_KEY,aSettings);
    m_server_data->allowedOrigins = GetByteArrayListFromSettingsInline(QTUTILS_CORE_HTTPSERVER_ALLOWED_ORIGINS_KEY,aSettings);

    this->AddStraightRoute("/qtutils_get_allowed_headers",[this](QTcpServer* a_cnctSrv, const QHttpServerRequest& a_request, QHttpServerResponder& a_responder){
        static_cast<void>(a_cnctSrv);
        handleAllowedHeadersRequest(a_request,a_responder);
        return true;
    });

    this->AddStraightRoute("/qtutils_get_allowed_origins",[this](QTcpServer* a_cnctSrv, const QHttpServerRequest& a_request, QHttpServerResponder& a_responder){
        static_cast<void>(a_cnctSrv);
        handleAllowedOriginsRequest(a_request,a_responder);
        return true;
    });

    this->AddStraightRoute("/qtutils_get_all_urls",[this](QTcpServer* a_cnctSrv, const QHttpServerRequest& a_request, QHttpServerResponder& a_responder){
        static_cast<void>(a_cnctSrv);
        handleAllUrlsRequest(a_request,a_responder);
        return true;
    });
}


bool HttpServer::handleRequest(const QHttpServerRequest& a_request, QHttpServerResponder& a_responder)
{
    const QUrl aUrl = a_request.url();
    const QString aPath = aUrl.path();
    
    // 1. try concrete straight correspondance
    const TypeHashS::const_iterator citerHS = m_server_data->straightRoutes.find(aPath);
    if(citerHS!=m_server_data->straightRoutes.cend()){
        return citerHS->second(m_server_data->m_pCurrentServer,a_request,a_responder);
    }
    
    // 2. try directory correspondance
    QString dirPath = aPath;
    qsizetype liOfDir = aPath.lastIndexOf('/');
    while(liOfDir>=0){
        dirPath = dirPath.left(liOfDir);
        TypeHashD::const_iterator citerHD = m_server_data->dirRoutes.find(dirPath);
        if(citerHD!=m_server_data->dirRoutes.cend()){
            return citerHD->second(m_server_data->m_pCurrentServer,a_request,a_responder,aPath.mid(liOfDir+1));
        }
        liOfDir = dirPath.lastIndexOf('/');
    }
    
    // 3. try glob reg exp
    TypeListRE::const_iterator citerLREG = m_server_data->globRegExpRoutes.cbegin();
    const TypeListRE::const_iterator citerLREGEnd = m_server_data->globRegExpRoutes.cend();
    for(;citerLREG!=citerLREGEnd;++citerLREG){
        const QRegularExpression reg(citerLREG->first);
        const QRegularExpressionMatch matchg = reg.match(aPath);
        if(matchg.hasMatch()){
            return citerLREG->second(m_server_data->m_pCurrentServer,a_request, a_responder,matchg);
        }
    }
     
    // 4. try wildcard reg exp
    TypeListRE::const_iterator citerLREW = m_server_data->wildcardRegExpRoutes.cbegin();
    const TypeListRE::const_iterator citerLREWEnd = m_server_data->wildcardRegExpRoutes.cend();
    for(;citerLREW!=citerLREWEnd;++citerLREW){
        const QRegularExpression rew = QRegularExpression::fromWildcard(citerLREW->first,Qt::CaseSensitive,QRegularExpression::UnanchoredWildcardConversion);
        const QRegularExpressionMatch matchw = rew.match(aPath);
        if(matchw.hasMatch()){
            return citerLREW->second(m_server_data->m_pCurrentServer,a_request, a_responder,matchw);
        }
    }
    
    // 5. any apperance
    TypeListAA::const_iterator citerLAA = m_server_data->anyAppearanceRoutes.cbegin();
    const TypeListAA::const_iterator citerLAAEnd = m_server_data->anyAppearanceRoutes.cend();
    for(;citerLAA!=citerLAAEnd;++citerLAA){
        if(aPath.contains(citerLAA->first)){
            return citerLAA->second(m_server_data->m_pCurrentServer,a_request, a_responder);
        }
    }
    
    // 6. any matcher
    TypeListAnM::const_iterator citerLAM = m_server_data->anyMatcherRoutes.cbegin();
    const TypeListAnM::const_iterator citerLAMEnd = m_server_data->anyMatcherRoutes.cend();
    for(;citerLAM!=citerLAMEnd;++citerLAM){
        const ::std::tuple<TypeHasMatch,void*,TypeClbkAnM>& aItem = *citerLAM;
        if( (::std::get<0>(aItem))(aUrl,::std::get<1>(aItem)) ){
            return (::std::get<2>(aItem))(m_server_data->m_pCurrentServer,a_request, a_responder,::std::get<1>(aItem));
        }
    }
    
    return false;
}


#if QT_VERSION < QT_VERSION_CHECK(6, 7, 0)
void HttpServer::missingHandler(const QHttpServerRequest& a_request, QHttpServerResponder&& a_responder)
#else
void HttpServer::missingHandler(const QHttpServerRequest& a_request, QHttpServerResponder& a_responder)
#endif
{
    static_cast<void>(a_request);
    static_cast<void>(a_responder);
}


const HttpServer::TypeHashS& HttpServer::getAllStraightRoutes()const
{
    return m_server_data->straightRoutes;
}


const HttpServer::TypeHashD& HttpServer::getAllDirRoutes()const
{
    return m_server_data->dirRoutes;
}


const HttpServer::TypeListRE& HttpServer::getAllGlobRegExpRoutes()const
{
    return m_server_data->globRegExpRoutes;
}


const HttpServer::TypeListRE& HttpServer::getAllWildcardRegExpRoutes()const
{
    return m_server_data->wildcardRegExpRoutes;
}


const HttpServer::TypeListAA& HttpServer::getAllAnyAppearanceRoutes()const
{
    return m_server_data->anyAppearanceRoutes;
}


void HttpServer::AddStraightRoute(const QString& a_path, const TypeClbkS& a_clbk)
{
    m_server_data->straightRoutes.insert({a_path,a_clbk});
}


void HttpServer::AddDirRoute(const QString& a_dirPath, const TypeClbkD& a_clbk)
{
    m_server_data->dirRoutes.insert({a_dirPath,a_clbk});
}


void HttpServer::AddGlobRegExpRoute(const QString& a_pattern, const TypeClbkRE& a_clbk)
{
    m_server_data->globRegExpRoutes.push_back(::std::pair<QString,TypeClbkRE>(a_pattern,a_clbk));
}


void HttpServer::AddWildcardRegExpRoute(const QString& a_pattern, const TypeClbkRE& a_clbk)
{
    m_server_data->wildcardRegExpRoutes.push_back(::std::pair<QString,TypeClbkRE>(a_pattern,a_clbk));
}


void HttpServer::AddAnyAppearanceRoute(const QString& a_pattern, const TypeClbkAA& a_clbk)
{
    m_server_data->anyAppearanceRoutes.push_back(::std::pair<QString,TypeClbkAA>(a_pattern,a_clbk));
}


void HttpServer::AddAnyMatcherRoute(const TypeHasMatch& a_hasMatch, void* a_ud, const TypeClbkAnM& a_clbk)
{
    m_server_data->anyMatcherRoutes.push_back(::std::tuple<TypeHasMatch,void*,TypeClbkAnM>(a_hasMatch,a_ud,a_clbk));
}


void HttpServer::SetAllowedHeaders(const ByteArrayList& a_allowedHeaders)
{
    QSettings aSettings;
    m_server_data->allowedHeaders = a_allowedHeaders;
    SetByteArrayListToSettingsInline(QTUTILS_CORE_HTTPSERVER_ALLOWED_HEADERS_KEY,a_allowedHeaders,&aSettings);
}


const ByteArrayList& HttpServer::getAllowedHeaders() const
{
    return m_server_data->allowedHeaders;
}


void HttpServer::SetAllowedOrigins(const ByteArrayList& a_allowedOrigins)
{
    QSettings aSettings;
    m_server_data->allowedOrigins = a_allowedOrigins;
    SetByteArrayListToSettingsInline(QTUTILS_CORE_HTTPSERVER_ALLOWED_ORIGINS_KEY,a_allowedOrigins,&aSettings);
}


const ByteArrayList& HttpServer::getAllowedOrigins() const
{
    return m_server_data->allowedOrigins;
}


bool HttpServer::checkAndFixResponceHeaders(const QHttpServerRequest& a_request, QHttpServerResponse* CPPUTILS_ARG_NN a_pResp) const
{
    return HttpServerCheckAndFixResponceHeadersInline1(a_request,m_server_data->allowedHeaders,m_server_data->allowedOrigins,a_pResp);
}


bool HttpServer::checkAndFixResponceHeaders(const TypeRestHeaders& a_vHeaders, QHttpServerResponse* CPPUTILS_ARG_NN a_pResp) const
{
    return HttpServerCheckAndFixResponceHeadersInlineRaw(a_vHeaders,m_server_data->allowedHeaders,m_server_data->allowedOrigins,a_pResp);
}


void HttpServer::SendResponseSoft(const QHttpServerResponse& a_responce, QHttpServerResponder& a_responder, QTcpServer* a_cnctSrv)
{
#ifdef _WIN32

    static_cast<void>(a_cnctSrv);
    QThread* const curThread = QThread::currentThread();
    if(curThread==(m_server_data->m_pHttpThread)){
        a_responder.sendResponse( a_responce );
    }  //  if(curThread==(m_server_data->m_pHttpThread)){
    else{
        QMetaObject::invokeMethod(this,[&a_responce,&a_responder]{
            QHttpServerResponder* const pResponder = new QHttpServerResponder(::std::move(a_responder));
            pResponder->sendResponse( a_responce );
            delete pResponder;
        },Qt::BlockingQueuedConnection);
    }  //  else of 'if(curThread==(m_server_data->m_pHttpThread)){'

#else

    QSslServer* const pSslServer = dynamic_cast<QSslServer*>(a_cnctSrv);
    if(pSslServer){
        a_responder.sendResponse( a_responce );
    }  //  if(pSslServer){
    else{
        QThread* const curThread = QThread::currentThread();
        if(curThread==(m_server_data->m_pHttpThread)){
            a_responder.sendResponse( a_responce );
        }  //  if(curThread==(m_server_data->m_pHttpThread)){
        else{
            QMetaObject::invokeMethod(this,[&a_responce,&a_responder]{
                QHttpServerResponder* const pResponder = new QHttpServerResponder(::std::move(a_responder));
                pResponder->sendResponse( a_responce );
                delete pResponder;
            },Qt::BlockingQueuedConnection);
        }  //  else of 'if(curThread==(m_server_data->m_pHttpThread)){'
    }  //  else of 'if(pSslServer){'

#endif
}


void HttpServer::SendResponseWeb(const QHttpServerRequest& a_request, QHttpServerResponse* CPPUTILS_ARG_NN a_responce_p, QHttpServerResponder& a_responder)
{
    HttpServerCheckAndFixResponceHeadersInline1(a_request,m_server_data->allowedHeaders,m_server_data->allowedOrigins,a_responce_p);
    a_responder.sendResponse( *a_responce_p );
}


void HttpServer::SendResponseWeb(const TypeRestHeaders& a_headers, QHttpServerResponse* CPPUTILS_ARG_NN a_responce_p, QHttpServerResponder& a_responder, QTcpServer* a_cnctSrv)
{

    QSslServer* const pSslServer = dynamic_cast<QSslServer*>(a_cnctSrv);
    if(pSslServer){
        HttpServerCheckAndFixResponceHeadersInlineRaw(a_headers,m_server_data->allowedHeaders,m_server_data->allowedOrigins,a_responce_p);
        a_responder.sendResponse( *a_responce_p );
    }  //  if(pSslServer){
    else{
        QThread* const curThread = QThread::currentThread();
        if(curThread==(m_server_data->m_pHttpThread)){
            HttpServerCheckAndFixResponceHeadersInlineRaw(a_headers,m_server_data->allowedHeaders,m_server_data->allowedOrigins,a_responce_p);
            a_responder.sendResponse( *a_responce_p );
        }  //  if(curThread==(m_server_data->m_pHttpThread)){
        else{
            QMetaObject::invokeMethod(this,[this,a_headers,a_responce_p,&a_responder]{
                QHttpServerResponder* const pResponder = new QHttpServerResponder(::std::move(a_responder));
                HttpServerCheckAndFixResponceHeadersInlineRaw(a_headers,m_server_data->allowedHeaders,m_server_data->allowedOrigins,a_responce_p);
                pResponder->sendResponse( *a_responce_p );
                delete pResponder;
            },Qt::BlockingQueuedConnection);
        }  //  else of 'if(curThread==(m_server_data->m_pHttpThread)){'
    }  //  else of 'if(pSslServer){'
}


void HttpServer::handleAllowedHeadersRequest(const QHttpServerRequest& a_request, QHttpServerResponder& a_responder)
{
    const QVariantList allowedVL = ByteArrayListToVariantListInline(m_server_data->allowedHeaders);
    const QJsonArray allowedJsonArray = QJsonArray::fromVariantList(allowedVL);
    const QJsonDocument allowedJsonDoc ( allowedJsonArray );
    const QByteArray allowedBA = allowedJsonDoc.toJson(QJsonDocument::Indented);
    QHttpServerResponse aResp(allowedBA,QHttpServerResponse::StatusCode::Ok);
    checkAndFixResponceHeaders(a_request,&aResp);
    a_responder.sendResponse(aResp);
}


void HttpServer::handleAllowedOriginsRequest(const QHttpServerRequest& a_request, QHttpServerResponder& a_responder)
{
    const QVariantList allowedVL = ByteArrayListToVariantListInline(m_server_data->allowedOrigins);
    const QJsonArray allowedJsonArray = QJsonArray::fromVariantList(allowedVL);
    const QJsonDocument allowedJsonDoc ( allowedJsonArray );
    const QByteArray allowedBA = allowedJsonDoc.toJson(QJsonDocument::Indented);
    QHttpServerResponse aResp(allowedBA,QHttpServerResponse::StatusCode::Ok);
    checkAndFixResponceHeaders(a_request,&aResp);
    a_responder.sendResponse(aResp);
}


void HttpServer::handleAllUrlsRequest(const QHttpServerRequest& a_request, QHttpServerResponder& a_responder)
{
    const QVariantList strRoutesVL = AnyDataFromAnyMapContainerToVariantListInline(m_server_data->straightRoutes);
    const QVariantList dirRoutesVL = AnyDataFromAnyMapContainerToVariantListInline(m_server_data->dirRoutes);
    const QVariantList regExpRoutesVL = AnyDataFromAnyMapContainerToVariantListInline(m_server_data->globRegExpRoutes);
    const QVariantList wldcardRegExpRoutesVL = AnyDataFromAnyMapContainerToVariantListInline(m_server_data->wildcardRegExpRoutes);
    const QVariantList anyApperanceRoutesVL = AnyDataFromAnyMapContainerToVariantListInline(m_server_data->anyAppearanceRoutes);
    const QVariantList anyMatcherRoutesVL = AnyDataFromAnyTupleContainerToVariantListInline(m_server_data->anyMatcherRoutes);
    const QVariantMap replyVariantMap = {
        {"straightRoutes",strRoutesVL},
        {"dirRoutes",dirRoutesVL},
        {"regExpRoutes",regExpRoutesVL},
        {"wildcardRegExpRoutes",wldcardRegExpRoutesVL},
        {"anyApperanceRoutes",anyApperanceRoutesVL},
        {"anyMatcherRoutes",anyMatcherRoutesVL}
    };
    const QJsonDocument replyJsonDoc = QJsonDocument(QJsonObject::fromVariantMap(replyVariantMap));
    const QByteArray replyBA = replyJsonDoc.toJson(QJsonDocument::Indented);
    QHttpServerResponse aResp(replyBA,QHttpServerResponse::StatusCode::Ok);
    checkAndFixResponceHeaders(a_request,&aResp);
    a_responder.sendResponse(aResp);
}


QTcpServer* HttpServer::CreateListenBindToTcpServer(quint16 a_port, const QHostAddress& a_address)
{
    QTcpServer* const server_p = new Server<QTcpServer>(m_server_data);
    const bool bListenRet = server_p->listen(a_address, a_port);
    if(!bListenRet){
        delete server_p;
        QtUtilsCritical()<<"Listen to port "<<a_port<<" failed";
        return nullptr;
    }
    const bool bBindRet = bind(server_p);
    if(!bBindRet){
        delete server_p;
        QtUtilsCritical()<<"Bind to server with port "<<a_port<<" failed";
        return nullptr;
    }
    return server_p;
}


QSslServer* HttpServer::CreateListenBindToSslServer(quint16 a_port, const QSslConfiguration& a_aSslConfig, const QHostAddress& a_address)
{
    QSslServer* const server_p = new Server<QSslServer>(m_server_data);
    server_p->setSslConfiguration(a_aSslConfig);
    const bool bListenRet = server_p->listen(a_address, a_port);
    if(!bListenRet){
        delete server_p;
        QtUtilsCritical()<<"Listen to port "<<a_port<<" failed";
        return nullptr;
    }
    const bool bBindRet = bind(server_p);
    if(!bBindRet){
        delete server_p;
        QtUtilsCritical()<<"Bind to server with port "<<a_port<<" failed";
        return nullptr;
    }
    return server_p;
}


#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)

quint16 HttpServer::listen(const QHostAddress& a_address, quint16 a_port)
{    
    QSslServer* const pSslServer = new QSslServer();
    if(pSslServer->listen(a_address, a_port)){
        if(bind(pSslServer)){
            return pSslServer->serverPort();
        }
        else{
            QtUtilsCritical()<<"Bind to server with port "<<a_port<<" failed";
            delete pSslServer;
        }
    }
    else{
        QtUtilsCritical()<<"Listen to port "<<a_port<<" failed";
        delete pSslServer;
    }
    
    return 0;
}


void HttpServer::sslSetup(const QSslConfiguration& a_sslConfiguration)
{
    const QList<QTcpServer*> vctServers = this->servers();
    const qsizetype serversCount = vctServers.size();
    for(qsizetype i(0); i<serversCount; ++i){
        QSslServer* const pSslServer = dynamic_cast<QSslServer*>(vctServers.at(i));
        if(pSslServer){
            pSslServer->setSslConfiguration(a_sslConfiguration);
            return;
        }
    }  //  for(qsizetype i(0); i<serversCount; ++i){

    QtUtilsCritical()<<"First call listen then sslSetup";
}


#endif



/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

HttpServer_p::~HttpServer_p()
{    
}


HttpServer_p::HttpServer_p()
    :
    m_pHttpThread(QThread::currentThread())
{
}

/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

QTUTILS_CORE_EXPORT ByteArrayList VariantListToByteArrayList(const QVariantList& a_listVL)
{
    return VariantListToByteArrayListInline(a_listVL);
}


QTUTILS_CORE_EXPORT ByteArrayList GetByteArrayListFromSettings(const QString& a_key, const QSettings& a_settings)
{
    return GetByteArrayListFromSettingsInline(a_key,a_settings);
}


QTUTILS_CORE_EXPORT void SetByteArrayListToSettings(const QString& a_key, const ByteArrayList& a_list, QSettings* CPPUTILS_ARG_NN a_settings_p)
{
    SetByteArrayListToSettingsInline(a_key,a_list,a_settings_p);
}



}}  //  namespace qtutils { namespace core{
