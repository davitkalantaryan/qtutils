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
#include <qtutils/disable_utils_warnings.h>
#include <QAbstractHttpServer>
#include <QString>
#include <QRegularExpressionMatch>


namespace qtutils { namespace core{

class CPPUTILS_DLL_PRIVATE HttpServer_p;

class QTUTILS_CORE_EXPORT HttpServer : public QAbstractHttpServer
{
public:
    typedef ::std::function<bool(const QHttpServerRequest& a_request, QHttpServerResponder& a_responder)> TypeClbkS;
    typedef ::std::function<bool(const QHttpServerRequest& a_request, const QString& a_fileName, QHttpServerResponder& a_responder)> TypeClbkD;
    typedef ::std::function<bool(const QHttpServerRequest& a_request, const QString& a_path, const QRegularExpressionMatch&,QHttpServerResponder& a_responder)> TypeClbkRE;
    typedef ::std::function<bool(const QHttpServerRequest& a_request, const QString& a_path, QHttpServerResponder& a_responder)> TypeClbkAM;
    typedef ::std::unordered_map<QString,TypeClbkS>         TypeHashS;
    typedef ::std::unordered_map<QString,TypeClbkD>         TypeHashD;  // directory routes
    typedef ::std::list<::std::pair<QString,TypeClbkRE> >   TypeListRE;
    typedef ::std::list<::std::pair<QString,TypeClbkAM> >   TypeListAM;
    
public:
    ~HttpServer() override;
    HttpServer();
    
    const TypeHashS&  getAllStraightRoutes()const;
    const TypeHashD&  getAllDirRoutes()const;
    const TypeListRE& getAllGlobRegExpRoutes()const;
    const TypeListRE& getAllWildcardRegExpRoutes()const;
    const TypeListAM& getAllAnyAppearanceRoutes()const;
    void AddStraightRoute(const QString& a_path, const TypeClbkS& a_clbk);
    void AddDirRoute(const QString& a_dirPath, const TypeClbkD& a_clbk);
    void AddGlobRegExpRoute(const QString& a_pattern, const TypeClbkRE& a_clbk);
    void AddWildcardRegExpRoute(const QString& a_pattern, const TypeClbkRE& a_clbk);
    void AddAnyAppearanceRoute(const QString& a_pattern, const TypeClbkAM& a_clbk);

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
