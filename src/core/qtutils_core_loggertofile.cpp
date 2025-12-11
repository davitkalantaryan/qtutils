//
// file:            qtutils_tools_loggertofile.cpp
// path:			src/tools/qtutils_tools_loggertofile.cpp
// created on:		2022 Oct 13
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#ifndef cinternal_unnamed_sema_wait_ms_needed
#define cinternal_unnamed_sema_wait_ms_needed
#endif
#include <qtutils/core/loggertofile.hpp>
#include <cinternal/unnamed_semaphore.h>
#include <cinternal/bistateflags.h>
#include <cinternal/disable_compiler_warnings.h>
#include <queue>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <qtutils/disable_utils_warnings.h>
#include <QFileInfo>
#include <QStandardPaths>
#include <cinternal/undisable_compiler_warnings.h>


namespace qtutils { namespace core{ namespace logger{


enum class CommandType{
    ApplyValues,
    NewData
};


struct SCmmndToFileThr{
    CommandType     type;
    LoggerData      data;
};


class CPPUTILS_DLL_PRIVATE LoggerToFile_p final
{
public:
    QFile                           m_logFile;
    QDate                           m_currentDate;
    QDir                            m_logsDir;
    cinternal_unnamed_sema_t        m_sema;
    ::std::queue<SCmmndToFileThr>   m_data;
    mutable ::std::mutex            m_dataMut;
    mutable ::std::shared_mutex     m_admMut;
    ::std::thread                   m_fileThread;
public:
    inline QString logFilePathInlineNoLock(const QDate& a_date)const;
    inline void OpenLogFileInline();
    void FileThreadFunction();
public:
    CPPUTILS_BISTATE_FLAGS_UN(
        threadShouldRun,
        fileOpened
    )m_flags;
};


ToFile::~ToFile()
{
    cinternal_unnamed_sema_destroy(&(m_logger_data_p->m_sema));
    delete m_logger_data_p;
}


ToFile::ToFile(const char* a_endStr)
    :
      Base(a_endStr),
      m_logger_data_p(new LoggerToFile_p())
{
    m_logger_data_p->m_flags.wr_all = CPPUTILS_BISTATE_MAKE_ALL_BITS_FALSE;
    cinternal_unnamed_sema_create(&(m_logger_data_p->m_sema),100);
}


void ToFile::Start()
{
    if(m_logger_data_p->m_flags.rd.threadShouldRun_true){
        return;
    }

    m_logger_data_p->m_flags.wr.threadShouldRun = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
    m_logger_data_p->m_fileThread = ::std::thread(&LoggerToFile_p::FileThreadFunction,m_logger_data_p);
}


void ToFile::Stop()
{
    if(m_logger_data_p->m_flags.rd.threadShouldRun_false){
        return;
    }
    m_logger_data_p->m_flags.wr.threadShouldRun = CPPUTILS_BISTATE_MAKE_BITS_FALSE;
    cinternal_unnamed_sema_post(&(m_logger_data_p->m_sema));
    m_logger_data_p->m_fileThread.join();
}


void ToFile::SetCurrentDate(const QDate& a_newDate)
{
    bool bDoAction = false;

    {  //  start lock guard
        ::std::lock_guard< ::std::shared_mutex > aGuard1(m_logger_data_p->m_admMut);
        if((m_logger_data_p->m_currentDate)!=a_newDate){
            m_logger_data_p->m_currentDate = a_newDate;
            bDoAction = true;
        }
    }  //  end lock guard

    if((m_logger_data_p->m_flags.rd.threadShouldRun_true)&&bDoAction){
        const SCmmndToFileThr aInf({CommandType::ApplyValues,LoggerData()});

        {  //  start lock guard
            ::std::lock_guard< ::std::mutex > aGuard2(m_logger_data_p->m_dataMut);
            m_logger_data_p->m_data.push(aInf);
        }  //  end lock guard

        cinternal_unnamed_sema_post(&(m_logger_data_p->m_sema));
    }  //  if((m_logger_data_p->m_flags.rd.threadShouldRun_true)&&bDoAction){
}


void ToFile::SetLogsDir(const QDir& a_logsDir)
{
    bool bDoAction = false;

    {  //  start lock guard
        ::std::lock_guard< ::std::shared_mutex > aGuard1(m_logger_data_p->m_admMut);
        if((m_logger_data_p->m_logsDir)!=a_logsDir){
            m_logger_data_p->m_logsDir = a_logsDir;
            bDoAction = true;
        }
    }  //  end lock guard

    if((m_logger_data_p->m_flags.rd.threadShouldRun_true)&&bDoAction){
        const SCmmndToFileThr aInf({CommandType::ApplyValues,LoggerData()});

        {  //  start lock guard
            ::std::lock_guard< ::std::mutex > aGuard2(m_logger_data_p->m_dataMut);
            m_logger_data_p->m_data.push(aInf);
        }  //  end lock guard

        cinternal_unnamed_sema_post(&(m_logger_data_p->m_sema));
    }  //  if((m_logger_data_p->m_flags.rd.threadShouldRun_true)&&bDoAction){
}


QDate ToFile::currentDate()const
{
    QDate aCurrDate;

    {
        ::std::shared_lock< ::std::shared_mutex > aGuard1(m_logger_data_p->m_admMut);
        aCurrDate = m_logger_data_p->m_currentDate;
    }

    return aCurrDate;
}


QDir ToFile::logsDir()const
{
    QDir aLogsDir;

    {
        ::std::shared_lock< ::std::shared_mutex > aGuard1(m_logger_data_p->m_admMut);
        aLogsDir = m_logger_data_p->m_logsDir;
    }

    return aLogsDir;
}


QString ToFile::logFilePath(const QDate& a_date)const
{
    QString aLogFilePath;

    {
        ::std::shared_lock< ::std::shared_mutex > aGuard1(m_logger_data_p->m_admMut);
        aLogFilePath = m_logger_data_p->logFilePathInlineNoLock(a_date);
    }

    return aLogFilePath;
}


QString ToFile::logFilePathCurrentDate()const
{
    QString aLogFilePath;

    {
        ::std::shared_lock< ::std::shared_mutex > aGuard1(m_logger_data_p->m_admMut);
        aLogFilePath = m_logger_data_p->logFilePathInlineNoLock(m_logger_data_p->m_currentDate);
    }

    return aLogFilePath;
}


void ToFile::LoggerClbk(CinternalLogCategory a_categoryEnm, const char* CPPUTILS_ARG_NN a_categoryStr, const char* CPPUTILS_ARG_NN a_log, size_t a_logStrLen)
{
    const SCmmndToFileThr aInf({CommandType::NewData,{a_categoryEnm,a_categoryStr,a_log,a_logStrLen}});

    {  //  start lock guard
        ::std::lock_guard< ::std::mutex > aGuard(m_logger_data_p->m_dataMut);
        m_logger_data_p->m_data.push(aInf);
        if(m_logger_data_p->m_flags.rd.threadShouldRun_false){
            if(m_logger_data_p->m_data.size()>100){
                m_logger_data_p->m_data.pop();
            }
        }  //  if(m_logger_data_p->m_flags.rd.threadShouldRun_false){
    }  //  end lock guard

    if(m_logger_data_p->m_flags.rd.threadShouldRun_true){
        cinternal_unnamed_sema_post(&(m_logger_data_p->m_sema));
    }
}


/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

inline QString LoggerToFile_p::logFilePathInlineNoLock(const QDate& a_date)const
{
    const QFileInfo aFileInfo(m_logsDir,"log_" + a_date.toString("yyyy.MM.dd.txt"));
    return aFileInfo.filePath();
}


inline void LoggerToFile_p::OpenLogFileInline()
{
    QString logFilePath;

    {  //  start lock guard
        ::std::shared_lock< ::std::shared_mutex > aGuard1(m_admMut);
        logFilePath = logFilePathInlineNoLock(m_currentDate);
    }

    m_logFile.setFileName(logFilePath);
    if(m_logFile.open(QIODevice::Append|QIODevice::Text)){
        m_flags.wr.fileOpened = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
    }  //  end lock guard
}


void LoggerToFile_p::FileThreadFunction()
{
    ::std::vector<SCmmndToFileThr> vectNextData;
    size_t unCount, ind;
    const SCmmndToFileThr* cpData;

    OpenLogFileInline();

    while(m_flags.rd.threadShouldRun_true){

        {  //  start lock guard
            ::std::lock_guard< ::std::mutex > aGuard(m_dataMut);
            while(m_data.size()>0){
                vectNextData.push_back(m_data.front());
                m_data.pop();
            }  //  while(m_data.size()>0){
        }  //  end lock guard

        unCount = vectNextData.size();
        cpData = vectNextData.data();
        for(ind=0;ind<unCount;++ind){
            switch(cpData[ind].type){
            case CommandType::ApplyValues:{
                if(m_flags.rd.fileOpened_true){
                    m_logFile.close();
                    m_flags.wr.fileOpened = CPPUTILS_BISTATE_MAKE_BITS_FALSE;
                    OpenLogFileInline();
                }  //  if(m_flags.rd.fileOpened_true){
            }break;
            case CommandType::NewData:{
                if(m_flags.rd.fileOpened_true && cpData[ind].data.m_log){
                    m_logFile.write(cpData[ind].data.m_log);
                    m_logFile.flush();
                }  //  if(m_flags.rd.fileOpened_true){
            }break;
            default:
                break;
            }  //  switch(cpData[ind].type){
        }  //  for(ind=0;ind<unCount;++ind){

        cinternal_unnamed_sema_wait(&m_sema);

    }  //  while(m_flags.rd.threadShouldRun_true){

    m_logFile.close();
    m_flags.wr.fileOpened = CPPUTILS_BISTATE_MAKE_BITS_FALSE;

}


}}}  //  namespace qtutils { namespace core{ namespace logger{
