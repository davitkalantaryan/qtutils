//
// file:			httpserver.hpp
// path:			${repositoryRoot}/include/qtutils/core/httpserver.hpp
// created on:		2023 Jun 18
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#pragma once

#include <kalantaryan/llfrontend/export_symbols.h>
#include <unordered_map>
#include <list>
#include <utility>
#include <functional>
#include <qtutils/disable_utils_warnings.h>
#include <QAbstractHttpServer>
#include <QString>


namespace qtutils { namespace core{

class CPPUTILS_DLL_PRIVATE HttpServer_p;

class LLFEKTR_EXPORT HttpServer : public QAbstractHttpServer
{
public:
    typedef ::std::function<bool(const QHttpServerRequest& a_request, QHttpServerResponder& a_responder)> TypeClbk;
    typedef ::std::function<bool(const QHttpServerRequest& a_request, const QString& a_fileName, QHttpServerResponder& a_responder)> TypeClbkD;
    typedef ::std::unordered_map<QString,TypeClbk>      TypeHashS;
    typedef ::std::unordered_map<QString,TypeClbkD>      TypeHashD;  // directory routes
    typedef ::std::list<::std::pair<QString,TypeClbk> > TypeListP;
    
public:
    ~HttpServer() override;
    HttpServer();
    
    const TypeHashS& getAllStraightRoutes()const;
    const TypeHashD& getAllDirRoutes()const;
    const TypeListP& getAllPatternRoutes()const;
    void AddStraightRoute(const QString& a_path, const TypeClbk& a_clbk);
    void AddDirRoute(const QString& a_dirPath, const TypeClbkD& a_clbk);
    void AddPatternRoute(const QString& a_pattern, const TypeClbk& a_clbk);

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
