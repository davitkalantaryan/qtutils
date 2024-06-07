//
// repo:            qtutils
// file:            main_networkacccessmanager_test01.cpp
// path:			src/tests/main_networkacccessmanager_test01.cpp
// created on:		2024 Jun 06
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//


#include <qtutils/core/networkaccessmanagerqu.hpp>
#include <qtutils/core/logger.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QCoreApplication>


#define TEST_ENDPOINT   "https://en3u24t1ju08q.x.pipedream.net"

using namespace qtutils;

int main(int a_argc, char* a_argv[])
{
    QCoreApplication aApp(a_argc, a_argv);
    network::AccessManager aMngr;

    const QUrl url = QString(TEST_ENDPOINT) + "/test1";
    QNetworkRequest request(url);

    const QVariantMap requestVM = {
        {"hostUser","kalantar"},
        {"customer_id",  24},
        {"customer_code", "blabla"},
    };

    network::Reply* const pReply1 = aMngr.postSingleton(request,requestVM,15000);
    ::QObject::connect(pReply1,&network::Reply::finished,&aApp,[](QTUTILS_CORE_NTDT_NSP QtUtilsNetReplyArg){
        QtUtilsDebug()<<" ";
    });

    aApp.exec();

    return 0;
}
