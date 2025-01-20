//
// file:            focust_threadls.cpp
// path:			src/core/focust_threadls.cpp
// created on:		2022 Jan 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/core/logger.hpp>
#include <cinternal/disable_compiler_warnings.h>
#include <new>
#include <qtutils/disable_utils_warnings.h>
#include <QFileInfo>
#include <QDateTime>
#include <cinternal/undisable_compiler_warnings.h>


namespace qtutils { namespace core{ namespace logger{


#define QTUTILS_CORE_LOGGER_NEW_CONTEXT     -2143


class CPPUTILS_DLL_PRIVATE DefaultLogDeleter final
{
public:
    ~DefaultLogDeleter();
};

struct MessageLogContextExtra{
    const char* m_realFileName;
    int         m_realLineNumber;
    int         m_logLevel;
};

static bool             s_bInited = false;
static Base*            s_pDefaultLogger = nullptr;
static QtMessageHandler s_defaultHandler = nullptr;

static void MessageHandlerStatic(QtMsgType a_type, const QMessageLogContext& a_context,const QString& a_message);


static void DeleteDefaultLogInline(void) noexcept {
    if(s_pDefaultLogger){
        delete s_pDefaultLogger;
        s_pDefaultLogger = nullptr;
    }
}


static void AddDefaultLoggerInline(void){
    if(!s_pDefaultLogger){
        s_pDefaultLogger = new Default();
    }
}


static inline CinternalLogCategory QtCategoryToCinternalInline(const QtMsgType& a_cat) noexcept {
    switch (a_cat) {
    case QtDebugMsg:
        return CinternalLogCategoryDebug;
    case QtWarningMsg:
        return CinternalLogCategoryWarning;
    case QtCriticalMsg:
        return CinternalLogCategoryCritical;
    case QtInfoMsg:
        return CinternalLogCategoryInfo;
    default:  // remains QtSystemMsg and QtCriticalMsg
        break;
    }  //  switch (a_type) {
    return CinternalLogCategoryFatal;
}


static inline QtMsgType CinternalLogCategoryQtLogTypeInline(const CinternalLogCategory& a_logCat){
    switch(a_logCat){
    case CinternalLogCategoryFatal:
        return QtFatalMsg;
    case CinternalLogCategoryCritical:
        return QtCriticalMsg;
    case CinternalLogCategoryWarning:
        return QtWarningMsg;
    case CinternalLogCategoryInfo:
        return QtInfoMsg;
    default:
        break;
    }  //  switch(a_logCat){
    return QtDebugMsg;
}


static inline void InitializeQtLogger(){
    if(s_bInited){
        return;
    }
    s_bInited = true;
    s_defaultHandler = qInstallMessageHandler(&MessageHandlerStatic);
    AddDefaultLoggerInline();
    CinternalLoggerRemoveDefaultlyAddedLogger();
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/


Base::~Base() noexcept
{
    CinternalLoggerRemoveLogger(m_loggerItem);
}


Base::Base(const char* a_cpcEndStr)
    :
    m_loggerItem(CinternalLoggerAddLogger(&Base::LoggerClbkSt,this,a_cpcEndStr))
{
    InitializeQtLogger();
    if(!m_loggerItem){
        throw ::std::bad_alloc();
    }
}


Base::Base(const Base& a_cM)
    :
    m_loggerItem(CinternalLoggerAddLogger(&Base::LoggerClbkSt,this,CinternalLoggerGetEndString(a_cM.m_loggerItem)))
{
    if(!m_loggerItem){
        throw ::std::bad_alloc();
    }
}


Base::Base(Base&& a_mM) noexcept
    :
    m_loggerItem(a_mM.m_loggerItem)
{
    a_mM.m_loggerItem = nullptr;
}


Base& Base::operator=(const Base& a_cM)
{
    const char* cpcNewEpcndStr = CinternalLoggerGetEndString(a_cM.m_loggerItem);
    const int nRet = CinternalLoggerSetEndString(m_loggerItem,cpcNewEpcndStr);
    if(nRet){
        throw ::std::bad_alloc();
    }
    return *this;
}


Base& Base::operator=(Base&& a_mM) noexcept
{
    CinternalLoggerItem* const loggerItem = m_loggerItem;
    m_loggerItem = a_mM.m_loggerItem;
    a_mM.m_loggerItem = loggerItem;
    return *this;
}


void Base::LoggerClbkSt(void* a_userData, enum CinternalLogCategory a_categoryEnm, const char* CPPUTILS_ARG_NN a_categoryStr, const char* CPPUTILS_ARG_NN a_log, size_t a_logStrLen)
{
    Base* const thisPtr = static_cast<Base*>(a_userData);
    thisPtr->LoggerClbk(a_categoryEnm,a_categoryStr,a_log,a_logStrLen);
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

void Default::LoggerClbk(CinternalLogCategory a_categoryEnm, const char* CPPUTILS_ARG_NN a_categoryStr, const char* CPPUTILS_ARG_NN a_log, size_t a_logStrLen)
{
    // QtMsgType a_type, const QMessageLogContext& a_context,const QString& a_message
    const QtMsgType qtLogType = CinternalLogCategoryQtLogTypeInline(a_categoryEnm);
    const QMessageLogContext aCtx("",-1,"",a_categoryStr);
    const QString aMessage = QString(a_log);

    static_cast<void>(a_logStrLen);
    (*s_defaultHandler)(qtLogType,aCtx,aMessage);
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

static void MessageHandlerStatic(QtMsgType a_type, const QMessageLogContext& a_context,const QString& a_message)
{
    int logLevel = 0;
    int nLine = a_context.line;
    const char* cpcFileName = a_context.file;
    const CinternalLogCategory logType = QtCategoryToCinternalInline(a_type);
    const ::std::string logMsg = a_message.toStdString();
    const char* const pcLogMsg = logMsg.c_str();
    if(nLine==QTUTILS_CORE_LOGGER_NEW_CONTEXT){
        const MessageLogContextExtra* const pExtraContext = (const MessageLogContextExtra*)(a_context.file);
        cpcFileName = pExtraContext->m_realFileName;
        nLine = pExtraContext->m_realLineNumber;
        logLevel = pExtraContext->m_logLevel;
    }  //  if(nLine==QTUTILS_CORE_LOGGER_NEW_CONTEXT){

    CinternalLoggerMakeLog(logLevel,a_context.category,
                           cpcFileName,nLine,a_context.function,
                           CinternalLogTypeCompleteLoggingWithPlaceAndFunc,logType,"%s",pcLogMsg);
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

DefaultLogDeleter::~DefaultLogDeleter()
{
    DeleteDefaultLogInline();
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

MessageLogger::~MessageLogger()
{
    delete m_extraContext;
}


static inline char* CreateExtraContextInline(MessageLogContextExtra** CPPUTILS_ARG_NN a_ppBuf,const char* a_fileName, int a_lineNumber,int a_logLevel){
    *a_ppBuf = new MessageLogContextExtra();
    (*a_ppBuf)->m_realFileName = a_fileName;
    (*a_ppBuf)->m_realLineNumber = a_lineNumber;
    (*a_ppBuf)->m_logLevel = a_logLevel;
    return (char*)(*a_ppBuf);
}


MessageLogger::MessageLogger(const char* a_fileName, int a_lineNumber, const char* a_functionName, const char* a_categoryName, int a_logLevel)
    :
    QMessageLogger(CreateExtraContextInline(&m_extraContext,a_fileName,a_lineNumber,a_logLevel),QTUTILS_CORE_LOGGER_NEW_CONTEXT,a_functionName,a_categoryName)
{
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/


QTUTILS_EXPORT Base* GetDefaultlyAddedLogger(void) noexcept
{
    return s_pDefaultLogger;
}


QTUTILS_EXPORT void RemoveDefaultlyAddedLogger(void) noexcept
{
    DeleteDefaultLogInline();
}


QTUTILS_EXPORT Base* AddDefaultLogger(const char* a_endStr)
{
    return new Default(a_endStr);
}


QTUTILS_EXPORT void RemoveLogger(Base* a_logger) noexcept
{
    if(a_logger){
        if(a_logger==s_pDefaultLogger){
            s_pDefaultLogger = nullptr;
        }  //  if(a_logger==s_pDefaultLogger){
        delete a_logger;
    }  //  if(a_logger){
}



}}}  //  namespace qtutils { namespace core{ namespace logger{
