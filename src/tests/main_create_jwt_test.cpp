//
// file:			main_create_jwt_test.cpp
// path:			src/tests/main_create_jwt_test.cpp
// created on:		2023 May 12
// created by:		Davit Kalantaryan (davit.kalantaryan1980@gmail.com)
//


#include <qtutils/core/create_jwt.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QDebug>


int main(void)
{
    QByteArray baSignature;
    const QVariantMap payloadVM = {
        {"email",QString("harut1225@gmail.com")},
        {"exp",1682268930},
        {"iat",1682254530},
        {"type",1},
    };
    const QByteArray jwtToken = ::qtutils::core::CreateJWT("RS256",payloadVM,::qtutils::core::GenerateJwtSecret01(),&baSignature);
    if(jwtToken.size()<1){
        qWarning()<<"Unable to generate JWT token";
        return 1;
    }
    
    qDebug()<<"jwtToken: "<<jwtToken;
    return 0;
}
