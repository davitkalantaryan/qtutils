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


class CPPUTILS_DLL_PRIVATE DefaultLogDeleter final
{
public:
    ~DefaultLogDeleter();
};

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
    if(!s_defaultHandler){
        AddDefaultLoggerInline();
        s_defaultHandler = qInstallMessageHandler(&MessageHandlerStatic);
        CinternalLoggerRemoveDefaultlyAddedLogger();
    }
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


void Base::LoggerClbkSt(void* a_userData, enum CinternalLogCategory a_categoryEnm, const char* CPPUTILS_ARG_NN a_categoryStr, char* CPPUTILS_ARG_NN a_log, size_t a_logStrLen)
{
    Base* const thisPtr = static_cast<Base*>(a_userData);
    thisPtr->LoggerClbk(a_categoryEnm,a_categoryStr,a_log,a_logStrLen);
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

void Default::LoggerClbk(CinternalLogCategory a_categoryEnm, const char* CPPUTILS_ARG_NN a_categoryStr, char* CPPUTILS_ARG_NN a_log, size_t a_logStrLen)
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
    const CinternalLogCategory logType = QtCategoryToCinternalInline(a_type);
    const ::std::string logMsg = a_message.toStdString();
    const char* const pcLogMsg = logMsg.c_str();
    CinternalLoggerMakeLog(0,a_context.category,a_context.file,a_context.line,a_context.file,CinternalLogTypeCompleteLoggingWithPlaceAndFunc,logType,"%s",pcLogMsg);
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

DefaultLogDeleter::~DefaultLogDeleter()
{
    DeleteDefaultLogInline();
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



}}}  //  namespace qtutils { namespace core{ namespace logger{
