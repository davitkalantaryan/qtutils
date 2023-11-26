//
// file:			main_create_jwt_test.cpp
// path:			src/tests/main_create_jwt_test.cpp
// created on:		2023 May 12
// created by:		Davit Kalantaryan (davit.kalantaryan1980@gmail.com)
//


#include <qtutils/core/create_jwt.hpp>
#include <stdio.h>
#include <qtutils/disable_utils_warnings.h>
#include <QDebug>


int main(int a_argc, char* a_argv[])
{
    
    bool bIsServer = false;
    
#if defined(CPPUTILS_DEBUG) && !defined(QTUTILS_SKIPJWT_PARSE_DEBUG)
    if(a_argc>2){
        fprintf(stdout,"Press any key then enter to continue! ");
        fflush(stdout);
        getchar();
    }
#endif
    
    if(a_argc>1){
        if((a_argv[1])[0]=='s'){
            bIsServer = true;
        }
    }
    
    QByteArray baSignature;
    const QVariantMap payloadVMUsr = {
        {"email","davit.kalantaryan@gmail.com"},
        {"exp",1682268930},
        {"iat",1682254530},
        {"roles",QVariantList({"ROLE_USER"})},
        {"type",1},
    };
    const QVariantMap payloadVMSrv = {
        {"iss","FocusT p01 llbackend"},
        {"exp",1682268930},
        {"iat",1682254530},
    };
    const QVariantMap& payloadVM = bIsServer?payloadVMSrv:payloadVMUsr;
    const QByteArray aSecret = ::qtutils::core::GenerateJwtSecret01();
    const QByteArray aSecretBase64 = aSecret.toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
    const QByteArray jwtToken = ::qtutils::core::CreateJWT("HS256",payloadVM,aSecretBase64,&baSignature);
    if(jwtToken.size()<1){
        qWarning()<<"Unable to generate JWT token";
        return 1;
    }
    
    qDebug()<<"\naSecretBase64: "<<aSecretBase64;
    qDebug()<<"\njwtToken: "<<jwtToken<<"\n";
    return 0;
}
