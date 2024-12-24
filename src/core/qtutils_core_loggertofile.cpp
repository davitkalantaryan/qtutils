//
// file:            qtutils_tools_loggertofile.cpp
// path:			src/tools/qtutils_tools_loggertofile.cpp
// created on:		2022 Oct 13
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/core/loggertofile.hpp>
#include <mutex>
#include <qtutils/disable_utils_warnings.h>
#include <QFileInfo>
#include <QStandardPaths>


namespace qtutils { namespace core{ namespace logger{



class CPPUTILS_DLL_PRIVATE LoggerToFile_p final
{
public:
    ::std::shared_ptr<QFile>    m_logFile;
    QDate                       m_currentDate;
    QDir                        m_logsDir;
    ::std::recursive_mutex      m_mutex;
public:
    inline QString logFilePathInline(const QDate& a_date)const;
    inline ::std::shared_ptr<QFile> CreateLogFileInline();
    inline void CreateLogFileNoCheckNoLockInline();
};



// m_logger.SetNewLogger(MessageHandlerSt,this);

ToFile::~ToFile()
{
    delete m_logger_data_p;
}


ToFile::ToFile()
    :
      m_logger_data_p(new LoggerToFile_p())
{
    const QDir dbDir = QDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    QFileInfo aFileInfo02(dbDir,"logs");

    m_logger_data_p->m_logsDir.setPath(aFileInfo02.filePath());
    m_logger_data_p->m_pOwner = nullptr;
}


void ToFile::RecreateLogFile()
{
    //return m_logger_data_p->CreateLogFileInline();
    ::std::lock_guard< ::std::recursive_mutex> aGuard(m_logger_data_p->m_mutex);
    m_logger_data_p->m_logFile = ::std::shared_ptr<QFile>();
    m_logger_data_p->CreateLogFileNoCheckNoLockInline();
}


void ToFile::SetCurrentDate(const QDate& a_newDate)
{
    ::std::lock_guard< ::std::recursive_mutex> aGuard(m_logger_data_p->m_mutex);
    m_logger_data_p->m_currentDate = a_newDate;
}


void ToFile::SetLogsDir(const QDir& a_logsDir)
{
    ::std::lock_guard< ::std::recursive_mutex> aGuard(m_logger_data_p->m_mutex);
    m_logger_data_p->m_logsDir = a_logsDir;
}


QDate ToFile::currentDate()const
{
    ::std::lock_guard< ::std::recursive_mutex> aGuard(m_logger_data_p->m_mutex);
    return m_logger_data_p->m_currentDate;
}


QDir ToFile::logsDir()const
{
    ::std::lock_guard<::std::recursive_mutex> aGuard(m_logger_data_p->m_mutex);
    return m_logger_data_p->m_logsDir;
}


QString ToFile::logFilePath(const QDate& a_date)const
{
    return m_logger_data_p->logFilePathInline(a_date);
}


QString ToFile::logFilePathCurrentDate()const
{
    QFile* const pLogFile = m_logger_data_p->m_logFile.get();
    if(pLogFile){
        return pLogFile->fileName();
    }
    return "";
}


void ToFile::LoggerClbk(CinternalLogCategory a_categoryEnm, const char* CPPUTILS_ARG_NN a_categoryStr, char* CPPUTILS_ARG_NN a_log, size_t a_logStrLen)
{
    const ::std::shared_ptr<QFile> logFile = m_logger_data_p->CreateLogFileInline();
    QFile* const logFile_p = logFile.get();

    if(logFile_p && logFile_p->isOpen()){
        logFile_p->write(a_log,static_cast<qint64>(a_logStrLen));
        return;
    }

    static_cast<void>(a_categoryEnm);
    static_cast<void>(a_categoryStr);
}



/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

void LoggerToFile_p::MessageHandler(QtMsgType a_msgType, const QMessageLogContext& a_ctx, const QString& a_msg)
{
    ::std::shared_ptr<QFile>  logFile = CreateLogFileInline();
    QFile* logFile_p = logFile.get();

    if(logFile_p && logFile_p->isOpen()){
        QTextStream out(logFile_p);
        out << a_msg<< "\n";
#if defined(QTUTILS_LOG_TO_FILE_CALL_DEFAULT_LOGS)
        QtMessageHandler defHandler = ::qtutils::Logger::DefaultHandler();
        (*defHandler)(a_msgType,a_ctx,a_msg);
#endif
    }
    else{
        QtMessageHandler defHandler = ::qtutils::Logger::DefaultHandler();
        (*defHandler)(a_msgType,a_ctx,a_msg);
    }

    m_extraLoggerClbk(m_pOwner,a_msgType,a_ctx,a_msg);
}


inline QString LoggerToFile_p::logFilePathInline(const QDate& a_date)const
{
    const QFileInfo aFileInfo(m_logsDir,"log_" + a_date.toString("yyyy.MM.dd.txt"));
    return aFileInfo.filePath();
}


inline void LoggerToFile_p::CreateLogFileNoCheckNoLockInline()
{
    if(!m_currentDate.isValid()){
        m_currentDate = QDate::currentDate();
    }

    if(!m_logsDir.exists()){
        m_logsDir.mkpath(m_logsDir.path());
    }

    QFile*const pfFileRet_p = new QFile(logFilePathInline(m_currentDate));
    pfFileRet_p->open(QIODevice::Append);
    if(!pfFileRet_p->isOpen()){
        delete pfFileRet_p;
        m_logFile = ::std::shared_ptr<QFile>();
        return;
    }

    m_logFile = ::std::shared_ptr<QFile>(pfFileRet_p);
}


inline ::std::shared_ptr<QFile> LoggerToFile_p::CreateLogFileInline()
{
    ::std::shared_ptr<QFile> pfFileRet = m_logFile;
    QFile* pfFileRet_p = pfFileRet.get();

    if(!pfFileRet_p){
        ::std::lock_guard<mutex_ml> aGuard(m_mutex);
        CreateLogFileNoCheckNoLockInline();
        pfFileRet = m_logFile;
    }  // if(!pfFileRet_p){

    return pfFileRet;
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

mutex_ml::mutex_ml()
{
	cinternal_lw_recursive_mutex_create(&m_mutex);
}

mutex_ml::~mutex_ml()
{
	cinternal_lw_recursive_mutex_destroy(&m_mutex);
}

void mutex_ml::lock()
{
	cinternal_lw_recursive_mutex_lock(&m_mutex);
}

void mutex_ml::unlock()
{
	cinternal_lw_recursive_mutex_unlock(&m_mutex);
}


}}}  //  namespace qtutils { namespace core{ namespace logger{
