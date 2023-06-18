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
#include <QRegularExpression>
#include <QRegularExpressionMatch>


namespace qtutils { namespace core{



class CPPUTILS_DLL_PRIVATE HttpServer_p
{
public:
    HttpServer_p();
    
public:
    HttpServer::TypeHashS   straightRoutes;
    HttpServer::TypeHashD   dirRoutes;
    HttpServer::TypeListP   globRegExpRoutes;
    HttpServer::TypeListP   wildcardRegExpRoutes;
    HttpServer::TypeListP   anyAppearanceRoutes;
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
    
    // 3. try glob reg exp
    TypeListP::const_iterator citerLREG = m_server_data->globRegExpRoutes.cbegin();
    const TypeListP::const_iterator citerLREGEnd = m_server_data->globRegExpRoutes.cend();
    for(;citerLREG!=citerLREGEnd;++citerLREG){
        const QRegularExpression reg(citerLREG->first);
        const QRegularExpressionMatch matchg = reg.match(aPath);
        if(matchg.hasMatch()){
            return citerLREG->second(a_request, aPath, a_responder);
        }
    }
    
    
    // 4. try wildcard reg exp
    TypeListP::const_iterator citerLREW = m_server_data->wildcardRegExpRoutes.cbegin();
    const TypeListP::const_iterator citerLREWEnd = m_server_data->wildcardRegExpRoutes.cend();
    for(;citerLREW!=citerLREWEnd;++citerLREW){
        const QRegularExpression rew = QRegularExpression::fromWildcard(citerLREW->first,Qt::CaseSensitive,QRegularExpression::UnanchoredWildcardConversion);
        const QRegularExpressionMatch matchw = rew.match(aPath);
        if(matchw.hasMatch()){
            return citerLREW->second(a_request, aPath, a_responder);
        }
    }
    
    
    // 5. any apperance
    TypeListP::const_iterator citerLAA = m_server_data->anyAppearanceRoutes.cbegin();
    const TypeListP::const_iterator citerLAAEnd = m_server_data->anyAppearanceRoutes.cend();
    for(;citerLAA!=citerLAAEnd;++citerLAA){
        if(aPath.contains(citerLAA->first)){
            return citerLAA->second(a_request, aPath, a_responder);
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


const HttpServer::TypeListP& HttpServer::getAllGlobRegExpRoutes()const
{
    return m_server_data->globRegExpRoutes;
}


const HttpServer::TypeListP& HttpServer::getAllWildcardRegExpRoutes()const
{
    return m_server_data->wildcardRegExpRoutes;
}


const HttpServer::TypeListP& HttpServer::getAllAnyAppearanceRoutes()const
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


void HttpServer::AddGlobRegExpRoute(const QString& a_pattern, const TypeClbkP& a_clbk)
{
    m_server_data->globRegExpRoutes.push_back(::std::pair<QString,TypeClbkP>(a_pattern,a_clbk));
}


void HttpServer::AddWildcardRegExpRoute(const QString& a_pattern, const TypeClbkP& a_clbk)
{
    m_server_data->wildcardRegExpRoutes.push_back(::std::pair<QString,TypeClbkP>(a_pattern,a_clbk));
}


void HttpServer::AddAnyAppearanceRoute(const QString& a_pattern, const TypeClbkP& a_clbk)
{
    m_server_data->anyAppearanceRoutes.push_back(::std::pair<QString,TypeClbkP>(a_pattern,a_clbk));
}



/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

HttpServer_p::HttpServer_p()
{
}



}}  //  namespace qtutils { namespace core{
