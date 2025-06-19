//
// file:			kalantaryan_llfrontend_hosting_httpserver.cpp
// path:			${repositoryRoot}/src/core/kalantaryan_llfrontend_hosting/kalantaryan_llfrontend_hosting_httpserver.cpp
// created on:		2023 Jun 18
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/core/httpserver.hpp>
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
#include <cinternal/undisable_compiler_warnings.h>


namespace qtutils { namespace core{


class CPPUTILS_DLL_PRIVATE HttpServer_p final
{
public:
    ~HttpServer_p();
    HttpServer_p();
    
public:
    HttpServer::TypeHashS   straightRoutes;
    HttpServer::TypeHashD   dirRoutes;
    HttpServer::TypeListRE  globRegExpRoutes;
    HttpServer::TypeListRE  wildcardRegExpRoutes;
    HttpServer::TypeListAA  anyAppearanceRoutes;
    HttpServer::TypeListAnM anyMatcherRoutes;
    ByteArrayList           allowedHeaders;
    ByteArrayList           allowedOrigins;    
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
    QSslServer*             m_pFirstServer;
#endif
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

    this->AddStraightRoute("/qtutils_get_allowed_headers",[this](const QHttpServerRequest& a_request, QHttpServerResponder& a_responder){
        handleAllowedHeadersRequest(a_request,a_responder);
        return true;
    });

    this->AddStraightRoute("/qtutils_get_allowed_origins",[this](const QHttpServerRequest& a_request, QHttpServerResponder& a_responder){
        handleAllowedOriginsRequest(a_request,a_responder);
        return true;
    });

    this->AddStraightRoute("/qtutils_get_all_urls",[this](const QHttpServerRequest& a_request, QHttpServerResponder& a_responder){
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
        return citerHS->second(a_request,a_responder);
    }
    
    // 2. try directory correspondance
    QString dirPath = aPath;
    qsizetype liOfDir = aPath.lastIndexOf('/');
    while(liOfDir>=0){
        dirPath = dirPath.left(liOfDir);
        TypeHashD::const_iterator citerHD = m_server_data->dirRoutes.find(dirPath);
        if(citerHD!=m_server_data->dirRoutes.cend()){
            return citerHD->second(a_request,a_responder,aPath.mid(liOfDir+1));
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
            return citerLREG->second(a_request, a_responder,matchg);
        }
    }
     
    // 4. try wildcard reg exp
    TypeListRE::const_iterator citerLREW = m_server_data->wildcardRegExpRoutes.cbegin();
    const TypeListRE::const_iterator citerLREWEnd = m_server_data->wildcardRegExpRoutes.cend();
    for(;citerLREW!=citerLREWEnd;++citerLREW){
        const QRegularExpression rew = QRegularExpression::fromWildcard(citerLREW->first,Qt::CaseSensitive,QRegularExpression::UnanchoredWildcardConversion);
        const QRegularExpressionMatch matchw = rew.match(aPath);
        if(matchw.hasMatch()){
            return citerLREW->second(a_request, a_responder,matchw);
        }
    }
    
    // 5. any apperance
    TypeListAA::const_iterator citerLAA = m_server_data->anyAppearanceRoutes.cbegin();
    const TypeListAA::const_iterator citerLAAEnd = m_server_data->anyAppearanceRoutes.cend();
    for(;citerLAA!=citerLAAEnd;++citerLAA){
        if(aPath.contains(citerLAA->first)){
            return citerLAA->second(a_request, a_responder);
        }
    }
    
    // 6. any matcher
    TypeListAnM::const_iterator citerLAM = m_server_data->anyMatcherRoutes.cbegin();
    const TypeListAnM::const_iterator citerLAMEnd = m_server_data->anyMatcherRoutes.cend();
    for(;citerLAM!=citerLAMEnd;++citerLAM){
        const ::std::tuple<TypeHasMatch,void*,TypeClbkAnM>& aItem = *citerLAM;
        if( (::std::get<0>(aItem))(aUrl,::std::get<1>(aItem)) ){
            return (::std::get<2>(aItem))(a_request, a_responder,::std::get<1>(aItem));
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


void HttpServer::SendResponse(const QHttpServerRequest& a_request, QHttpServerResponse* CPPUTILS_ARG_NN a_responce_p, QHttpServerResponder& a_responder)
{
    QHttpServerResponse aResponce = ::std::move(*a_responce_p);
    HttpServerCheckAndFixResponceHeadersInline1(a_request,m_server_data->allowedHeaders,m_server_data->allowedOrigins,&aResponce);
    a_responder.sendResponse( aResponce );
}


void HttpServer::SendResponse(const TypeRestHeaders& a_headers, QHttpServerResponse* CPPUTILS_ARG_NN a_responce_p, QHttpServerResponder& a_responder)
{
    QHttpServerResponse aResponce = ::std::move(*a_responce_p);
    HttpServerCheckAndFixResponceHeadersInlineRaw(a_headers,m_server_data->allowedHeaders,m_server_data->allowedOrigins,&aResponce);
    a_responder.sendResponse( aResponce );
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


#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)

quint16 HttpServer::listen(const QHostAddress& a_address, quint16 a_port)
{
    if(m_server_data->m_pFirstServer){
        const quint16 listPort = m_server_data->m_pFirstServer->serverPort();
        if(listPort>0){
            return listPort;
        }
    }  //  if(m_server_data->m_pFirstServer){
    else{
        m_server_data->m_pFirstServer = new QSslServer();
    }
    
    if(m_server_data->m_pFirstServer->listen(a_address, a_port)){
        return m_server_data->m_pFirstServer->serverPort();
    }
    
    return -1;
}


void HttpServer::sslSetup(const QSslConfiguration& a_sslConfiguration)
{
    if(!(m_server_data->m_pFirstServer)){
        m_server_data->m_pFirstServer = new QSslServer();
    }
    m_server_data->m_pFirstServer->setSslConfiguration(a_sslConfiguration);
}


#endif



/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

HttpServer_p::~HttpServer_p()
{    
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
    delete m_pFirstServer;
#endif
}


HttpServer_p::HttpServer_p()
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
    :
    m_pFirstServer(nullptr)
#endif
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
