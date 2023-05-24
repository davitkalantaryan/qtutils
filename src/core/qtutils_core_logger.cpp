//
// file:            focust_threadls.cpp
// path:			src/core/focust_threadls.cpp
// created on:		2022 Jan 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#include <qtutils/core/logger.hpp>
#include <cpputils/inscopecleaner.hpp>
#include <cpputils/mutex_ml.hpp>
#include <mutex>
#include <qtutils/disable_utils_warnings.h>
#include <QFileInfo>
#include <QDateTime>


namespace qtutils{

static QtMessageHandler s_defaultHandler    = nullptr;
static Logger_p* s_pFirstLogger             = nullptr;
static ::cpputils::mutex_ml s_logsMutex;

static void MessageHandlerStatic(QtMsgType a_type, const QMessageLogContext& a_context,const QString& a_message);

static void MessageHandlerStaticForNull(void*,QtMsgType, const QMessageLogContext &,const QString &)
{
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

class CPPUTILS_DLL_PRIVATE Logger_p final
{
public:
    Logger_p(const Logger::TypeLogger& a_logger, void* a_pOwner);
    ~Logger_p();
    void MessageHandler(QtMsgType a_type, const QMessageLogContext& a_context,const QString& a_message);

private:
    Logger_p*           m_prev;

public:
    Logger_p*           m_next;
    void*               m_pOwner;
    Logger::TypeLogger  m_logger;
};


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

Logger::Logger(const TypeLogger& a_logger, void* a_pOwner)
    :
      m_logger_data_p(new Logger_p(a_logger?a_logger:&MessageHandlerStaticForNull,a_pOwner))
{
}


Logger::~Logger()
{
    delete m_logger_data_p;
}


void Logger::SetNewLogger(const TypeLogger& a_logger, void* a_pOwner)
{
    m_logger_data_p->m_logger = a_logger?a_logger:&MessageHandlerStaticForNull;
    m_logger_data_p->m_pOwner = a_pOwner;
}


QtMessageHandler Logger::DefaultHandler()
{
    QtMessageHandler retHandler = nullptr;

    {  //  start lock
        ::std::lock_guard<::cpputils::mutex_ml> aGuard(s_logsMutex);

        if(s_defaultHandler){
            retHandler = s_defaultHandler;
        }
        else{
            retHandler = s_defaultHandler = qInstallMessageHandler(&MessageHandlerStatic);
            qInstallMessageHandler(s_defaultHandler);
        }

    }  //  end locks

    return retHandler;
}


QString Logger::FileAndLineString(const char* a_fileName, int a_line)
{
    if(a_line>0){
        return QString("fl:") + QFileInfo(a_fileName).fileName() + QString(",ln:") + QString::number(a_line);
    }
    return "";
}


QString Logger::FileLineAndFunctionString(const char* a_fileName, int a_line, const char* a_cpcFunction)
{
    if(a_line>0){
        return FileAndLineString(a_fileName,a_line) + QString(",fn:\"") + a_cpcFunction+ "\": ";
    }
    return "";
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

void Logger_p::MessageHandler(QtMsgType a_type, const QMessageLogContext& a_context,const QString& a_message)
{
    QString msgCntx = QDateTime::currentDateTime().toString() + ": ";

    switch (a_type) {
    case QtDebugMsg:
        msgCntx += "debug:    ";
        break;
    case QtWarningMsg:
        msgCntx += "warning:  ";
        msgCntx += Logger::FileAndLineString(a_context.file,a_context.line) + ": ";
        break;
    case QtFatalMsg:
        msgCntx += "fatal:    ";
        msgCntx += Logger::FileLineAndFunctionString(a_context.file,a_context.line,a_context.function);
        break;
    case QtInfoMsg:
        msgCntx += "info:     ";
        break;
    default:  // remains QtSystemMsg and QtCriticalMsg
        msgCntx += "critical: ";
        msgCntx += Logger::FileLineAndFunctionString(a_context.file,a_context.line,a_context.function);
        break;
    }


    m_logger(m_pOwner,a_type,a_context,msgCntx+a_message);
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

Logger_p::Logger_p(const Logger::TypeLogger& a_logger, void* a_pOwner)
    :
      m_prev(nullptr),
      m_pOwner(a_pOwner),
      m_logger(a_logger)
{
    ::std::lock_guard<::cpputils::mutex_ml> aGuard(s_logsMutex);

    if(s_pFirstLogger){
        s_pFirstLogger->m_prev = this;
    }
    else{
        if(!s_defaultHandler){
            s_defaultHandler = qInstallMessageHandler(&MessageHandlerStatic);
        }
    }

    m_next = s_pFirstLogger;
    s_pFirstLogger = this;
}


Logger_p::~Logger_p()
{
    ::std::lock_guard<::cpputils::mutex_ml> aGuard(s_logsMutex);

    if(m_prev){m_prev->m_next = m_next;}
    if(m_next){m_next->m_prev = m_prev;}
    if(this==s_pFirstLogger){
        qInstallMessageHandler(s_defaultHandler);
        s_defaultHandler = nullptr;
        s_pFirstLogger = nullptr;
    }
}


static void MessageHandlerStatic(QtMsgType a_type, const QMessageLogContext& a_context,const QString& a_message)
{
    Logger_p* pLogger;
    ::std::lock_guard<::cpputils::mutex_ml> aGuard(s_logsMutex);
    pLogger = s_pFirstLogger;
    while(pLogger){
        pLogger->MessageHandler(a_type,a_context,a_message);
        pLogger = pLogger->m_next;
    }
}


}  // namespace qtutils{
