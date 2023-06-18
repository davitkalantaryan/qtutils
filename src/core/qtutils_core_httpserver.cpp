//
// file:			kalantaryan_llfrontend_hosting_httpserver.cpp
// path:			${repositoryRoot}/src/core/kalantaryan_llfrontend_hosting/kalantaryan_llfrontend_hosting_httpserver.cpp
// created on:		2023 Jun 18
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/core/httpserver.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QHttpServerRequest>
#include <QHttpServerResponder>
#include <QHttpServerResponse>
#include <QUrl>


namespace qtutils { namespace core{



class CPPUTILS_DLL_PRIVATE HttpServer_p
{
public:
    HttpServer_p();
    
public:
    HttpServer::TypeHashS    straightRoutes;
    HttpServer::TypeHashD    dirRoutes;
    HttpServer::TypeListP    patternRoutes;
};


/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/


HttpServer::~HttpServer()
{
    delete m_server_data;
}


HttpServer::HttpServer()
    :
      m_server_data(new HttpServer_p())
{
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
    const qsizetype	liOfDirPlus1 = aPath.lastIndexOf('/')+1;
    const QString dirPath = aPath.left(liOfDirPlus1);
    TypeHashD::const_iterator citerHD = m_server_data->dirRoutes.find(dirPath);
    if(citerHD!=m_server_data->dirRoutes.cend()){
        return citerHD->second(a_request,aPath.mid(liOfDirPlus1),a_responder);
    }
    
    // 3. try patterns
    TypeListP::const_iterator citerLP = m_server_data->patternRoutes.cbegin();
    const TypeListP::const_iterator citerLPEnd = m_server_data->patternRoutes.cend();
    for(;citerLP!=citerLPEnd;++citerLP){
        if(aPath.contains(citerLP->first)){
            return citerLP->second(a_request, a_responder);
        }
    }
    
    return false;
}



void HttpServer::missingHandler(const QHttpServerRequest& a_request, QHttpServerResponder&& a_responder)
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


const HttpServer::TypeListP& HttpServer::getAllPatternRoutes()const
{
    return m_server_data->patternRoutes;
}


void HttpServer::AddStraightRoute(const QString& a_path, const TypeClbk& a_clbk)
{
    m_server_data->straightRoutes.insert({a_path,a_clbk});
}


void HttpServer::AddDirRoute(const QString& a_dirPath, const TypeClbkD& a_clbk)
{
    m_server_data->dirRoutes.insert({a_dirPath,a_clbk});
}


void HttpServer::AddPatternRoute(const QString& a_pattern, const TypeClbk& a_clbk)
{
    m_server_data->patternRoutes.push_back(::std::pair<QString,TypeClbk>(a_pattern,a_clbk));
}



/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

HttpServer_p::HttpServer_p()
{
}



}}  //  namespace qtutils { namespace core{
