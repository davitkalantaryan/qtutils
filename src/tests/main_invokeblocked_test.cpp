//
// file:		main_q_enums_test.cpp
// path:		src/tests/main_q_enums_test.cpp
// created on:	2024 May 06
// creatd by:	Davit Kalantaryan (davit.kalantaryan@desy.de)
//


#include <qtutils/core/clsinvokeblocked.hpp>
#include <thread>
#include <mutex>
#include <iostream>
#include <stdio.h>
#include <qtutils/disable_utils_warnings.h>
#include <QCoreApplication>
#include <QThread>
#include <QDebug>

class MyThread : public QThread{
    void run() override;
};


static QObject*     s_pObject=nullptr;
static ::qtutils::core::blocking::Carrier*  s_pCarier = nullptr;
static bool s_rawThreadCallExecuted = false;


void MyThread::run()
{
    ::std::cout<<"Qt Thread id: "<< ::std::this_thread::get_id()<< ::std::endl;
    
    QObject* pObject = new QObject();
    s_pObject = pObject;
    exec();
    s_pObject = nullptr;
    delete pObject;
}


static void RawThreadFunctionStatic()
{
    ::std::cout<<"Std Thread id: "<< ::std::this_thread::get_id()<< ::std::endl;
    
    ::std::lock_guard<::qtutils::core::blocking::Carrier> aGuard(*s_pCarier);
    
    while(!s_pObject){
        QThread::sleep(2);
    }
    
    ::qtutils::core::blocking::CInvoke(s_pCarier,s_pObject,[](){
        ::std::lock_guard<::qtutils::core::blocking::Carrier> aGuard(*s_pCarier);
        ::std::cout<<"Thread id 02: "<< ::std::this_thread::get_id()<< ::std::endl;
    });
    
    s_rawThreadCallExecuted = true;
}


int main(int a_argc, char* a_argv[])
{
    ::qtutils::core::blocking::Carrier aCarier;
    fprintf(stdout,"Press any key then enter to continue! ");
    fflush(stdout);
    getchar();
    
    QCoreApplication aApp(a_argc,a_argv);
    MyThread aQtThread;
    
    ::std::cout<<"Main Thread id: "<< ::std::this_thread::get_id()<< ::std::endl;
    
    s_pCarier = &aCarier;
    
    //::qtutils::core::blocking::LockGuard aGuard(s_pCarier);
    
    aQtThread.start();
    ::std::thread aStdThread(&RawThreadFunctionStatic);
    
    while(!s_pObject){
        QThread::sleep(2);
    }
    
    ::qtutils::core::blocking::CInvoke(s_pCarier,s_pObject,[](){
        ::std::lock_guard<::qtutils::core::blocking::Carrier> aGuard(*s_pCarier);
        ::std::cout<<"Thread id 01: "<< ::std::this_thread::get_id()<< ::std::endl;
    });
    
    while(!s_rawThreadCallExecuted){
        QThread::sleep(2);
    }
    
    aStdThread.join();
    aQtThread.quit();
    aQtThread.wait();
            
	return 0;
}
