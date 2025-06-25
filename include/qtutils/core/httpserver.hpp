//
// file:			httpserver.hpp
// path:			${repositoryRoot}/include/qtutils/core/httpserver.hpp
// created on:		2023 Jun 18
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#pragma once

#include <qtutils/export_symbols.h>
#include <qtutils/core/http_data01.hpp>
#include <cinternal/disable_compiler_warnings.h>
#include <unordered_map>
#include <list>
#include <utility>
#include <functional>
#include <tuple>
#include <qtutils/disable_utils_warnings.h>
#include <QHttpServerRequest>
#include <QHttpServerResponder>
#include <QAbstractHttpServer>
#include <QString>
#include <QList>
#include <QByteArray>
#include <QPair>
#include <QSettings>
#include <QVariantList>
#include <QRegularExpressionMatch>
#include <QSslServer>
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
#include <QHttpHeaders>
#endif
#include <cinternal/undisable_compiler_warnings.h>


namespace qtutils { namespace core{

#define QTUTILS_CORE_HTTPSERVER_ALLOWED_HEADERS_KEY "qtutils/core/allowed_headers"
#define QTUTILS_CORE_HTTPSERVER_ALLOWED_ORIGINS_KEY "qtutils/core/allowed_origins"

class CPPUTILS_DLL_PRIVATE HttpServer_p;


class QTUTILS_CORE_EXPORT HttpServer : public QAbstractHttpServer
{
public:
    typedef ::std::function<bool(const QUrl& a_url, void*)> TypeHasMatch;
    typedef ::std::function<bool(const QHttpServerRequest& a_request, QHttpServerResponder& a_responder)> TypeClbkS;
    typedef ::std::function<bool(const QHttpServerRequest& a_request, QHttpServerResponder& a_responder, const QString& a_fileName)> TypeClbkD;
    typedef ::std::function<bool(const QHttpServerRequest& a_request, QHttpServerResponder& a_responder, const QRegularExpressionMatch& a_reMatch)> TypeClbkRE;
    typedef ::std::function<bool(const QHttpServerRequest& a_request, QHttpServerResponder& a_responder)> TypeClbkAA;
    typedef ::std::function<bool(const QHttpServerRequest& a_request, QHttpServerResponder& a_responder, void* a_ud)> TypeClbkAnM;
    typedef ::std::unordered_map<QString,TypeClbkS>                     TypeHashS;
    typedef ::std::unordered_map<QString,TypeClbkD>                     TypeHashD;  // directory routes
    typedef ::std::list<::std::pair<QString,TypeClbkRE> >               TypeListRE;
    typedef ::std::list<::std::pair<QString,TypeClbkAA> >               TypeListAA;
    typedef ::std::list<::std::tuple<TypeHasMatch,void*,TypeClbkAnM> >  TypeListAnM;
    
public:
    virtual ~HttpServer() override;
    HttpServer();
    HttpServer(const HttpServer&)=delete;
    HttpServer(HttpServer&)=delete;
    HttpServer& operator=(const HttpServer&)=delete;
    HttpServer& operator=(HttpServer&)=delete;
    
    const TypeHashS&  getAllStraightRoutes()const;
    const TypeHashD&  getAllDirRoutes()const;
    const TypeListRE& getAllGlobRegExpRoutes()const;
    const TypeListRE& getAllWildcardRegExpRoutes()const;
    const TypeListAA& getAllAnyAppearanceRoutes()const;
    const TypeListAnM& getAllAnyMatcherRoutes()const;
    void AddStraightRoute(const QString& a_path, const TypeClbkS& a_clbk);
    void AddDirRoute(const QString& a_dirPath, const TypeClbkD& a_clbk);
    void AddGlobRegExpRoute(const QString& a_pattern, const TypeClbkRE& a_clbk);
    void AddWildcardRegExpRoute(const QString& a_pattern, const TypeClbkRE& a_clbk);
    void AddAnyAppearanceRoute(const QString& a_pattern, const TypeClbkAA& a_clbk);
    void AddAnyMatcherRoute(const TypeHasMatch& a_hasMatch, void* a_ud, const TypeClbkAnM& a_clbk);
    void SetAllowedHeaders(const ByteArrayList& a_allowedHeaders);
    const ByteArrayList& getAllowedHeaders() const;
    void SetAllowedOrigins(const ByteArrayList& a_allowedOrigins);
    const ByteArrayList& getAllowedOrigins() const;
    bool checkAndFixResponceHeaders(const TypeRestHeaders& a_vHeaders, QHttpServerResponse* CPPUTILS_ARG_NN a_pResp)const;
    bool checkAndFixResponceHeaders(const QHttpServerRequest& a_request, QHttpServerResponse* CPPUTILS_ARG_NN a_pResp)const;
    void SendResponse(const QHttpServerRequest& a_request, QHttpServerResponse* CPPUTILS_ARG_NN a_responce_p, QHttpServerResponder& a_responder, bool a_isSsl);
    void SendResponse(const TypeRestHeaders& a_headers, QHttpServerResponse* CPPUTILS_ARG_NN a_responce_p, QHttpServerResponder& a_responder, bool a_isSsl);
    QTcpServer* CreateListenBindToTcpServer(quint16 a_port=0, const QHostAddress& a_address = QHostAddress::Any);
    QSslServer* CreateListenBindToSslServer(quint16 a_port=0, const QHostAddress& a_address = QHostAddress::Any);
        
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
    quint16 listen(const QHostAddress& a_address = QHostAddress::Any, quint16 a_port=0);
    void sslSetup(const QSslConfiguration& a_sslConfiguration);
#endif

protected:
    virtual bool handleRequest(const QHttpServerRequest& a_request, QHttpServerResponder& a_responder) override;    
#if QT_VERSION < QT_VERSION_CHECK(6, 7, 0)
    virtual void missingHandler(const QHttpServerRequest& a_request, QHttpServerResponder&& a_responder) override;
#else
    virtual void missingHandler(const QHttpServerRequest& a_request, QHttpServerResponder& a_responder) override;
#endif
    virtual void handleAllowedHeadersRequest(const QHttpServerRequest& a_request, QHttpServerResponder& a_responder);
    virtual void handleAllowedOriginsRequest(const QHttpServerRequest& a_request, QHttpServerResponder& a_responder);
    virtual void handleAllUrlsRequest(const QHttpServerRequest& a_request, QHttpServerResponder& a_responder);
    void CheckAndFixThreadAffinityOfSockets();
        
private:
    HttpServer_p* const     m_server_data;
};


QTUTILS_CORE_EXPORT ByteArrayList VariantListToByteArrayList(const QVariantList& a_listVL);
QTUTILS_CORE_EXPORT ByteArrayList GetByteArrayListFromSettings(const QString& a_key, const QSettings& a_settings);
QTUTILS_CORE_EXPORT void SetByteArrayListToSettings(const QString& a_key, const ByteArrayList& a_list, QSettings* CPPUTILS_ARG_NN a_settings_p);


}}  //  namespace qtutils { namespace core{
