//
// file:			main_parse_jwt_test.cpp
// path:			src/tests/main_parse_jwt_test.cpp
// created on:		2022 Oct 19
// created by:		Davit Kalantaryan (davit.kalantaryan1980@gmail.com)
//


#include <qtutils/core/parse_jwt.hpp>
#include <qtutils/core/global_functions.hpp>
#include <stdio.h>
#include <string.h>
#include <qtutils/disable_utils_warnings.h>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QJsonArray>

//static const char*const s_cpcSpcToken = "eyJ0eXAiOiJKV1QiLCJhbGciOiJSUzI1NiJ9.eyJpYXQiOjE2ODIyNTQ1MzAsImV4cCI6MTY4MjI2ODkzMCwicm9sZXMiOlsiUk9MRV9VU0VSIl0sImVtYWlsIjoiaGFydXQxMjI1QGdtYWlsLmNvbSIsInR5cGUiOjF9.wrUMvJdCsm6gpF4PEFE0Bzj-2L_-h2pZ83LN4yAr3HeAg3SKwdnY1eTeUT88WbYq5K79DDIsicPj6EXHFfzf3YysbGycZJikQtViGn1jI6sL3DM_5anEKiwQbWA3J5YHwuiMjMgZwfHjHAivewd2t5xvGqlt9CrAXG5qqPIbGuuKjfsj4ogjuYT9MtEIvC39FIFyXuShKQZP2EY7pZIro2x5lTD3TNiVgC-c2OWa8QN0i07WbSmT1ea-JDzhpCnZEgLRa2WA3eGVI_U6qoSdx_gwZjMQeAGiD3A-smq7UnqHjr0J2BDPGcSubFze708oRcqyDRgwESRI9TpRkcsexg";
  static const char*const s_cpcSpcToken = "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJlbWFpbCI6ImhhcnV0MTIyNUBnbWFpbC5jb20iLCJleHAiOjE2ODIyNjg5MzAsImlhdCI6MTY4MjI1NDUzMCwicm9sZXMiOlsiUk9MRV9VU0VSIl0sInR5cGUiOjF9.7eqjpPAOcgxbRGkVn5AIeZR8EKZ4PqkA6nuzf-hiR1c";


int main(void)
{
    QByteArray baSignature;
    const QList<QJsonObject> inpList = ::qtutils::core::ParseJWT(s_cpcSpcToken,&baSignature);
    if(inpList.size()!=2){
        qWarning()<<"Bad JWT token";
        return 1;
    }
    
    qDebug()<<"inpList:  "<<inpList;
    printf("token: %s\n",s_cpcSpcToken);
    printf("  header\n");
    fflush(stdout);
    QString aString;
    ::qtutils::core::SerializeJsonObject(inpList[0],[&aString](const char* a_cpcStr){
        if(a_cpcStr[0]){
            aString += QString(a_cpcStr);
        }
    },false,"   ");
    puts(aString.toStdString().c_str());
    printf("  payload\n");
    fflush(stdout);
    aString = QString();
    ::qtutils::core::SerializeJsonObject(inpList[1],[&aString](const char* a_cpcStr){
        if(a_cpcStr[0]){
            aString += QString(a_cpcStr);
        }
    },false,"   ");
    puts(aString.toStdString().c_str());
    fflush(stdout);
    printf("  signature\n");
    fflush(stdout);
    printf("    %s\n",baSignature.constData());
    fflush(stdout);
    return 0;
}
