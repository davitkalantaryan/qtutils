//
// file:			httpserver.hpp
// path:			${repositoryRoot}/include/qtutils/core/httpserver.hpp
// created on:		2023 Jun 18
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#pragma once

#include <qtutils/export_symbols.h>
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
#include <QRegularExpressionMatch>


namespace qtutils { namespace core{

class CPPUTILS_DLL_PRIVATE HttpServer_p;

class QTUTILS_CORE_EXPORT HttpServer : public QAbstractHttpServer
{
public:
    typedef ::std::function<bool(const QUrl& a_url, void*)> TypeHasMatch;
    typedef ::std::function<bool(const QHttpServerRequest& a_request, QHttpServerResponder& a_responder)> TypeClbkS;
    typedef ::std::function<bool(const QHttpServerRequest& a_request, QHttpServerResponder& a_responder, const QString& a_fileName)> TypeClbkD;
    typedef ::std::function<bool(const QHttpServerRequest& a_request, QHttpServerResponder& a_responder, const QRegularExpressionMatch& a_reMatch)> TypeClbkRE;
    typedef ::std::function<bool(const QHttpServerRequest& a_request, QHttpServerResponder& a_responder, const QString& a_path)> TypeClbkAA;
    typedef ::std::function<bool(const QHttpServerRequest& a_request, QHttpServerResponder& a_responder, const QString& a_path, void* a_ud)> TypeClbkAnM;
    typedef ::std::unordered_map<QString,TypeClbkS>                     TypeHashS;
    typedef ::std::unordered_map<QString,TypeClbkD>                     TypeHashD;  // directory routes
    typedef ::std::list<::std::pair<QString,TypeClbkRE> >               TypeListRE;
    typedef ::std::list<::std::pair<QString,TypeClbkAA> >               TypeListAA;
    typedef ::std::list<::std::tuple<TypeHasMatch,void*,TypeClbkAnM> >  TypeListAnM;
    
public:
    virtual ~HttpServer() override;
    HttpServer();
    
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

protected:
    virtual bool handleRequest(const QHttpServerRequest& a_request, QHttpServerResponder& a_responder) override;
    virtual void missingHandler(const QHttpServerRequest& a_request, QHttpServerResponder&& a_responder) override;
    
private:
    HttpServer(const HttpServer&)=delete;
    HttpServer(HttpServer&&)=delete;
    HttpServer& operator=(const HttpServer&)=delete;
    HttpServer& operator=(HttpServer&&)=delete;
    
private:
    HttpServer_p*const      m_server_data;
};


}}  //  namespace qtutils { namespace core{
