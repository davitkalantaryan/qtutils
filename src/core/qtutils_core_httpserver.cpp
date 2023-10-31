//
// file:			kalantaryan_llfrontend_hosting_httpserver.cpp
// path:			${repositoryRoot}/src/core/kalantaryan_llfrontend_hosting/kalantaryan_llfrontend_hosting_httpserver.cpp
// created on:		2023 Jun 18
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/core/httpserver.hpp>
#include <qtutils/disable_utils_warnings.h>
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
    HttpServer::TypeListRE  globRegExpRoutes;
    HttpServer::TypeListRE  wildcardRegExpRoutes;
    HttpServer::TypeListAA  anyAppearanceRoutes;
    HttpServer::TypeListAnM anyMatcherRoutes;
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
            return citerLAA->second(a_request, a_responder, aPath);
        }
    }
    
    // 6. any matcher
    TypeListAnM::const_iterator citerLAM = m_server_data->anyMatcherRoutes.cbegin();
    const TypeListAnM::const_iterator citerLAMEnd = m_server_data->anyMatcherRoutes.cend();
    for(;citerLAM!=citerLAMEnd;++citerLAM){
        const ::std::tuple<TypeHasMatch,void*,TypeClbkAnM>& aItem = *citerLAM;
        if( (::std::get<0>(aItem))(aUrl,::std::get<1>(aItem)) ){
            return (::std::get<2>(aItem))(a_request, a_responder, aPath,::std::get<1>(aItem));
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



/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

HttpServer_p::HttpServer_p()
{
}



}}  //  namespace qtutils { namespace core{
