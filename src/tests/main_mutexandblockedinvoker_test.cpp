//
// repo:        qtutils
// file:		main_mutexandblockedinvoker_test.cpp
// path:		src/tests/main_mutexandblockedinvoker_test.cpp
// created on:	2024 Oct 29
// creatd by:	Davit Kalantaryan (davit.kalantaryan@desy.de)
//


#include <qtutils/core/mutexandblockedinvoker.hpp>
#include <thread>
#include <mutex>
#include <iostream>
#include <stdio.h>
#include <qtutils/disable_utils_warnings.h>
#include <QCoreApplication>
#include <QThread>
#include <QDebug>
#ifdef CPPUTILS_PRESENT
#include <cpputils/deadlockfreemutexes.hpp>
typedef ::cpputils::mutex::DeadlockFreeMutexes< ::std::recursive_mutex> DeadLockFreeRecursive;
typedef ::qtutils::core::mutexandblockedinvoker::ArrayMutex< DeadLockFreeRecursive > MutexToUse;
typedef ::cpputils::orderedcalls::Guard<MutexToUse >    LockGuard;
#define MUTEX_PTR_TO_GUARD_ARGS(_mutex_ptr)     (_mutex_ptr), 0
#else
typedef ::qtutils::core::mutexandblockedinvoker::SimpleMutex<::std::recursive_mutex>  MutexToUse;
typedef ::std::lock_guard<MutexToUse>   LockGuard;
#define MUTEX_PTR_TO_GUARD_ARGS(_mutex_ptr)     (*(_mutex_ptr))
#endif


class MyThread : public QThread{
    void run() override;
};


static QObject*     s_pObject=nullptr;
static MutexToUse*  s_pCarier = nullptr;
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
    
    LockGuard aGuard(MUTEX_PTR_TO_GUARD_ARGS(s_pCarier));
    
    while(!s_pObject){
        QThread::sleep(2);
    }
    
    ::qtutils::core::mutexandblockedinvoker::InvokeBlocked(s_pCarier,s_pObject,[](){
        LockGuard aGuard(MUTEX_PTR_TO_GUARD_ARGS(s_pCarier));
        ::std::cout<<"Thread id 02: "<< ::std::this_thread::get_id()<< ::std::endl;
    });
    
    s_rawThreadCallExecuted = true;
}


int main(int a_argc, char* a_argv[])
{
      
#ifdef CPPUTILS_PRESENT
    ::std::recursive_mutex mutex1;
    ::std::recursive_mutex mutex2;
    ::std::recursive_mutex mutex3;
    ::std::recursive_mutex mutex4;
    ::std::recursive_mutex mutex5;
    const ::std::vector<::std::recursive_mutex* >  vMutexes({&mutex1,&mutex2,&mutex3,&mutex4,&mutex5});
    DeadLockFreeRecursive aMut1(vMutexes);
    //::qtutils::core::mutexandblockedinvoker::ArrayBaseMutex<DeadLockFreeRecursive,size_t> aMut2(vMutexes);
    MutexToUse aCarier (1);
#else
    MutexToUse aCarier;
#endif
    
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
    
    ::qtutils::core::mutexandblockedinvoker::InvokeBlocked(s_pCarier,s_pObject,[](){
        LockGuard aGuard(MUTEX_PTR_TO_GUARD_ARGS(s_pCarier));
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
