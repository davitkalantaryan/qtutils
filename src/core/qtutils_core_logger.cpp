//
// file:            focust_threadls.cpp
// path:			src/core/focust_threadls.cpp
// created on:		2022 Jan 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#include <qtutils/core/logger.hpp>
#include <cpputils/inscopecleaner.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QFileInfo>
#include <QDateTime>


namespace qtutils{

static Logger_p* s_pLogger = nullptr;

static void MessageHandlerStatic(QtMsgType a_type, const QMessageLogContext& a_context,const QString& a_message);

static void MessageHandlerStaticForNull(QtMsgType, const QMessageLogContext &,const QString &)
{
    qDebug()<<"";
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

class CPPUTILS_DLL_PRIVATE Logger_p
{
public:
    void MessageHandler(QtMsgType a_type, const QMessageLogContext& a_context,const QString& a_message);

public:
    QtMessageHandler    m_handleBefore;
    QtMessageHandler    m_handleCurrent;
};


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

Logger::Logger()
    :
      m_logger_data_p(new Logger_p())
{
    s_pLogger = this->m_logger_data_p;
    m_logger_data_p->m_handleCurrent = m_logger_data_p->m_handleBefore = qInstallMessageHandler(&MessageHandlerStatic);
}


Logger::~Logger()
{
    qInstallMessageHandler(m_logger_data_p->m_handleBefore);
    s_pLogger = nullptr;
    delete m_logger_data_p;
}


QtMessageHandler Logger::SetNewLogger(const QtMessageHandler& a_logger)
{
    QtMessageHandler logger = m_logger_data_p->m_handleCurrent;
    m_logger_data_p->m_handleCurrent = a_logger?a_logger:&MessageHandlerStaticForNull;
    return logger;
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


    m_handleCurrent(a_type,a_context,msgCntx+a_message);
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

static void MessageHandlerStatic(QtMsgType a_type, const QMessageLogContext& a_context,const QString& a_message)
{
    s_pLogger->MessageHandler(a_type,a_context,a_message);
}


}  // namespace qtutils{
