//
// file:            logger.hpp
// path:			include/qtutils/core/logger.hpp
// created on:		2022 Jan 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once


#include <qtutils/export_symbols.h>
#include <cinternal/logger.h>
#include <cinternal/disable_compiler_warnings.h>
#include <qtutils/disable_utils_warnings.h>
#include <QDebug>
#include <QMessageLogger>
#include <cinternal/undisable_compiler_warnings.h>


namespace qtutils { namespace core{ namespace logger{


struct MessageLogContextExtra;


class QTUTILS_EXPORT MessageLogger : public QMessageLogger
{
public:
    ~MessageLogger();
    MessageLogger(const char* a_fileName, int a_lineNumber, const char* a_functionName, const char* a_categoryName, int a_logLevel);
    MessageLogger(const MessageLogger&)=delete;
    MessageLogger(MessageLogger&&)=delete;
    MessageLogger& operator=(const MessageLogger&)=delete;
    MessageLogger& operator=(MessageLogger&&)=delete;
public:
    MessageLogContextExtra*     m_extraContext;
};

#define QTUTILS_CORE_LOGGER_SIMPLE              (-2140)

#define QtUtilsDebugNVC(_category)              QMessageLogger(__FILE__, __LINE__, "",_category).debug()
#define QtUtilsDebugNV                          QMessageLogger(__FILE__, __LINE__, "").debug
#define QtUtilsDebug                            QMessageLogger(__FILE__, __LINE__, __FUNCTION__).debug
#define QtUtilsDebugCategory(_category)         QMessageLogger(__FILE__, __LINE__, __FUNCTION__,_category).debug()
#define QtUtilsInfo                             QMessageLogger("",-1, "").info
#define QtUtilsInfoV                            QMessageLogger(__FILE__, __LINE__, "").info
#define QtUtilsInfoVV                           QMessageLogger(__FILE__, __LINE__, __FUNCTION__).info
#define QtUtilsWarning                          QMessageLogger(__FILE__, __LINE__, __FUNCTION__).warning
#define QtUtilsCritical                         QMessageLogger(__FILE__, __LINE__, __FUNCTION__).critical
#define QtUtilsFatal                            QMessageLogger(__FILE__, __LINE__, __FUNCTION__).fatal
#define QtUtilsSimpleLog(_qdebug)               QMessageLogger("", QTUTILS_CORE_LOGGER_SIMPLE, "")._qdebug()
#define QtUtilsSimpleLogCat(_qdebug,_category)  QMessageLogger("", QTUTILS_CORE_LOGGER_SIMPLE, "",_category)._qdebug()
#define QtUtilsDebugLogLvlCat(_ll,_cat)         ::qtutils::core::logger::MessageLogger(__FILE__, __LINE__, __FUNCTION__,_cat,_ll).debug()
#define QtUtilsDebugLogLvl(_ll)                 QtUtilsDebugLogLvlCat(_ll,"")


class QTUTILS_EXPORT Base
{
public:
    virtual ~Base() noexcept;
    Base(const char* a_cpcEndStr = "");
    Base(const Base& a_cM);
    Base(Base&& a_mM) noexcept;
    Base& operator=(const Base& a_cM);
    Base& operator=(Base&& a_mM) noexcept;

private:
    virtual void LoggerClbk(CinternalLogCategory a_categoryEnm, const char* CPPUTILS_ARG_NN a_categoryStr, const char* CPPUTILS_ARG_NN a_log, size_t a_logStrLen);

private:
    static void LoggerClbkSt(void* a_userData, enum CinternalLogCategory a_categoryEnm, const char* CPPUTILS_ARG_NN a_categoryStr, const char* CPPUTILS_ARG_NN a_log, size_t a_logStrLen);

private:
    CinternalLoggerItem*    m_loggerItem;
};


class QTUTILS_EXPORT Default : public Base
{
public:
    virtual ~Default() noexcept override = default;
    using Base::Base;

protected:
    virtual void LoggerClbk(CinternalLogCategory a_categoryEnm, const char* CPPUTILS_ARG_NN a_categoryStr, const char* CPPUTILS_ARG_NN a_log, size_t a_logStrLen) override;
};


QTUTILS_EXPORT QtMsgType CinternalLogCategoryQtLogType(const CinternalLogCategory& a_logCat) noexcept;
QTUTILS_EXPORT CinternalLogCategory QtCategoryToCinternal(const QtMsgType& a_cat) noexcept;


}}}  //  namespace qtutils { namespace core{ namespace logger{
