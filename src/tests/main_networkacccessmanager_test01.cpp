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

    const QUrl url1 = QString(TEST_ENDPOINT) + "/test_singleton1";
    const QUrl url2 = QString(TEST_ENDPOINT) + "/test_singleton2";
    const QUrl url3 = QString(TEST_ENDPOINT) + "/test_dependent1";
    const QUrl url4 = QString(TEST_ENDPOINT) + "/test_dependent2";
    const QUrl url5 = QString(TEST_ENDPOINT) + "/test_simple1";
    const QUrl url6 = QString(TEST_ENDPOINT) + "/test_simple2";
    const QUrl url7 = QString(TEST_ENDPOINT) + "/test_any1";
    const QUrl url8 = QString(TEST_ENDPOINT) + "/test_any2";
    const QUrl url9 = QString(TEST_ENDPOINT) + "/test_any3";
    QNetworkRequest request1(url1);
    QNetworkRequest request2(url2);
    QNetworkRequest request3(url3);
    QNetworkRequest request4(url4);
    QNetworkRequest request5(url5);
    QNetworkRequest request6(url6);
    QNetworkRequest request7(url7);
    QNetworkRequest request8(url8);
    QNetworkRequest request9(url9);

    const QVariantMap requestVM = {
        {"hostUser","kalantar"},
        {"customer_id",  24},
        {"customer_code", "blabla"},
    };

    const auto replyClbk = [](QTUTILS_CORE_NTDT_NSP QtUtilsNetReplyArg a_reply){
        ::qtutils::network::Reply*const pReplyHandler(a_reply.get());
        QNetworkReply*const pQtNetworkReply = pReplyHandler->qtNetworkReply();
        const QByteArray responseByteArray = pQtNetworkReply->readAll();
        QtUtilsDebug().noquote().nospace()<<"url:"<<pQtNetworkReply->url().toString()<<", responseByteArray: \""<<responseByteArray<<"\"";
    };

    network::Reply* const pReply1 = aMngr.postSingleton(request1,requestVM,15000);
    ::QObject::connect(pReply1,&network::Reply::finished,&aApp,replyClbk);

    network::Reply* const pReply2 = aMngr.postSingleton(request2,requestVM,15000);
    ::QObject::connect(pReply2,&network::Reply::finished,&aApp,replyClbk);

    network::Reply* const pReply3 = aMngr.postDependent(request3,requestVM,15000);
    ::QObject::connect(pReply3,&network::Reply::finished,&aApp,replyClbk);

    network::Reply* const pReply4 = aMngr.postDependent(request4,requestVM,15000);
    ::QObject::connect(pReply4,&network::Reply::finished,&aApp,replyClbk);

    network::Reply* const pReply5 = aMngr.post(request5,requestVM,15000);
    ::QObject::connect(pReply5,&network::Reply::finished,&aApp,replyClbk);

    network::Reply* const pReply6 = aMngr.post(request6,requestVM,15000);
    ::QObject::connect(pReply6,&network::Reply::finished,&aApp,replyClbk);

    network::Reply* const pReply7 = aMngr.postAny(network::CallType::SimpleDropSeed,request7,requestVM,15000,nullptr,1);
    ::QObject::connect(pReply7,&network::Reply::finished,&aApp,replyClbk);

    network::Reply* const pReply8 = aMngr.postAny(network::CallType::SimpleDropSeed,request8,requestVM,15000,nullptr,1);
    ::QObject::connect(pReply8,&network::Reply::finished,&aApp,replyClbk);

    network::Reply* const pReply9 = aMngr.postAny(network::CallType::SimpleKeepSeed,request9,requestVM,15000,nullptr,1);
    ::QObject::connect(pReply9,&network::Reply::finished,&aApp,replyClbk);

    aApp.exec();

    return 0;
}
