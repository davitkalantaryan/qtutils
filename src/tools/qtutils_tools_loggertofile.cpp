//
// file:            qtutils_tools_loggertofile.cpp
// path:			src/tools/qtutils_tools_loggertofile.cpp
// created on:		2022 Oct 13
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/tools/loggertofile.hpp>
#define cinternal_lw_recursive_mutex_create_needed
#include <cinternal/lw_mutex_recursive.h>
#include <mutex>
#include <qtutils/disable_utils_warnings.h>
#include <QFileInfo>
#include <QStandardPaths>


namespace qtutils{ namespace tools{


class CPPUTILS_DLL_PRIVATE mutex_ml final{
public:
	mutex_ml();
	~mutex_ml();
	void lock();
	void unlock();
	
private:
	cinternal_lw_recursive_mutex_t m_mutex;
};


class CPPUTILS_DLL_PRIVATE LoggerToFile_p final
{
public:
    mutex_ml					m_mutex;
    Logger::TypeLogger          m_extraLoggerClbk;
    void*                       m_pOwner;
    ::std::shared_ptr<QFile>    m_logFile;
    QDate                       m_currentDate;
    QDir                        m_logsDir;
    ::qtutils::Logger           m_logger;
public:
    void MessageHandler(QtMsgType a_msgType, const QMessageLogContext& a_ctx, const QString& a_msg);
    inline ::std::shared_ptr<QFile> CreateLogFileInline();
    inline void CreateLogFileNoCheckNoLockInline();
};


static void NoLogDefaultClbk(void*,QtMsgType,const QMessageLogContext&, const QString&)
{
}


static void MessageHandlerSt(void* a_pLogger, QtMsgType a_msgType, const QMessageLogContext& a_ctx, const QString& a_msg)
{
    if(a_pLogger){
        LoggerToFile_p*const pLogger = static_cast<LoggerToFile_p*>(a_pLogger);
        pLogger->MessageHandler(a_msgType, a_ctx,a_msg);
    }
    else{
        QtMessageHandler defHandler = ::qtutils::Logger::DefaultHandler();
        (*defHandler)(a_msgType,a_ctx,a_msg);
    }
}


// m_logger.SetNewLogger(MessageHandlerSt,this);

LoggerToFile::~LoggerToFile()
{
    m_logger_data_p->m_logger.SetNewLogger([](void*,QtMsgType a_msgType,const QMessageLogContext& a_ctx, const QString& a_msg){
        QtMessageHandler defHandler = ::qtutils::Logger::DefaultHandler();
        (*defHandler)(a_msgType,a_ctx,a_msg);
    });

    delete m_logger_data_p;
}


LoggerToFile::LoggerToFile()
    :
      m_logger_data_p(new LoggerToFile_p())
{
    const QDir dbDir = QDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    QFileInfo aFileInfo02(dbDir,"logs");

    m_logger_data_p->m_logsDir.setPath(aFileInfo02.filePath());
    m_logger_data_p->m_extraLoggerClbk = &NoLogDefaultClbk;
    m_logger_data_p->m_pOwner = nullptr;
    m_logger_data_p->m_logger.SetNewLogger(&MessageHandlerSt,m_logger_data_p);
}


void LoggerToFile::RecreateLogFile()
{
    //return m_logger_data_p->CreateLogFileInline();
    ::std::lock_guard<mutex_ml> aGuard(m_logger_data_p->m_mutex);
    m_logger_data_p->m_logFile = ::std::shared_ptr<QFile>();
    m_logger_data_p->CreateLogFileNoCheckNoLockInline();
}


void LoggerToFile::SetExtraLogger(const Logger::TypeLogger& a_logger, void* a_pOwner)
{
    m_logger_data_p->m_extraLoggerClbk = a_logger?a_logger:(&NoLogDefaultClbk);
    m_logger_data_p->m_pOwner = a_pOwner;
}


void LoggerToFile::SetCurrentDate(const QDate& a_newDate)
{
    ::std::lock_guard<mutex_ml> aGuard(m_logger_data_p->m_mutex);
    m_logger_data_p->m_currentDate = a_newDate;
}


void LoggerToFile::SetLogsDir(const QDir& a_logsDir)
{
    ::std::lock_guard<mutex_ml> aGuard(m_logger_data_p->m_mutex);
    m_logger_data_p->m_logsDir = a_logsDir;
}


QDate LoggerToFile::currentDate()const
{
    ::std::lock_guard<mutex_ml> aGuard(m_logger_data_p->m_mutex);
    return m_logger_data_p->m_currentDate;
}


Logger::TypeLogger LoggerToFile::extraLogger()const
{
    return m_logger_data_p->m_extraLoggerClbk;
}


void* LoggerToFile::extraLogUserData()const
{
    return m_logger_data_p->m_pOwner;
}


QDir LoggerToFile::logsDir()const
{
    ::std::lock_guard<mutex_ml> aGuard(m_logger_data_p->m_mutex);
    return m_logger_data_p->m_logsDir;
}


QString LoggerToFile::logFilePath()const
{
    QFile* const pLogFile = m_logger_data_p->m_logFile.get();
    if(pLogFile){
        return pLogFile->fileName();
    }
    return "";
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


inline void LoggerToFile_p::CreateLogFileNoCheckNoLockInline()
{
    if(!m_currentDate.isValid()){
        m_currentDate = QDate::currentDate();
    }

    if(!m_logsDir.exists()){
        m_logsDir.mkpath(m_logsDir.path());
    }

    const QFileInfo aFileInfo(m_logsDir,"log_" + m_currentDate.toString("yyyy.MM.dd.txt"));
    QFile*const pfFileRet_p = new QFile(aFileInfo.filePath());
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


}}  //  namespace qtutils{ namespace tools{
