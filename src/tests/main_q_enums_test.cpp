//
// file:		main_q_enums_test.cpp
// path:		src/tests/main_q_enums_test.cpp
// created on:	2024 May 06
// creatd by:	Davit Kalantaryan (davit.kalantaryan@desy.de)
//


#include <qtutils/core/networkaccessmanagerqu.hpp>
#include <qtutils/core/settings.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QDebug>


int main(void)
{
    const int cnMaxValueToCheck = static_cast<int>(QHttpServerRequest::Method::AnyKnown)+10;
    
    for(int i(0); i<cnMaxValueToCheck;++i){
        const QByteArray verb = ::qtutils::network::HttpRequestMethodToByteArray(static_cast<QHttpServerRequest::Method>(i));
        if(verb.size()>0){
            qDebug().noquote().nospace()<<"HttpRequestMethodToByteArray("<<i<<")="<<verb;
        }
    }
	return 0;
}
