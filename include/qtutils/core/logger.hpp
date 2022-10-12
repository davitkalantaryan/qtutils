//
// file:            logger.hpp
// path:			include/qtutils/core/logger.hpp
// created on:		2022 Jan 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once


#include <qtutils/export_symbols.h>
#include <functional>
#include <qtutils/disable_utils_warnings.h>
#include <QDebug>
#include <QMessageLogger>


namespace qtutils { 

class CPPUTILS_DLL_PRIVATE Logger_p;

#define QtUtilsDebugNV                      QMessageLogger(__FILE__, __LINE__, __FUNCTION__).debug
#define QtUtilsDebug()                      (QtUtilsDebugNV()<< ::qtutils::Logger::FileAndLineString(__FILE__,__LINE__) << ": ")
#define QtUtilsDebugV()                     (QtUtilsDebugNV()<< ::qtutils::Logger::FileLineAndFunctionString(__FILE__,__LINE__,__FUNCTION__) << ": ")
#define QtUtilsInfo                         QMessageLogger(__FILE__, __LINE__, __FUNCTION__).info
#define QtUtilsWarning                      QMessageLogger(__FILE__, __LINE__, __FUNCTION__).warning
#define QtUtilsCriticalRaw(_fl,_ln,_fnc)    QMessageLogger(_fl, _ln, _fnc).critical()
#define QtUtilsCritical                     QMessageLogger(__FILE__, __LINE__, __FUNCTION__).critical
#define QtUtilsFatal                        QMessageLogger(__FILE__, __LINE__, __FUNCTION__).fatal


class QTUTILS_EXPORT Logger final
{
public:
    typedef ::std::function<void(void*,QtMsgType, const QMessageLogContext &, const QString &)> TypeLogger;
public:
    Logger(const TypeLogger& a_logger=nullptr, void* a_pOwner=nullptr);
    Logger(const Logger&)=delete;
    Logger(Logger&&)=delete;
    ~Logger();
    Logger& operator=(const Logger&)=delete;
    Logger& operator=(Logger&&)=delete;

    void SetNewLogger(const TypeLogger& a_logger, void* a_pOwner=nullptr);

    static QtMessageHandler DefaultHandler();
    static QString          FileAndLineString(const char* a_fileName, int a_line);
    static QString          FileLineAndFunctionString(const char* a_fileName, int a_line, const char* a_cpcFunction);

private:
    Logger_p*const      m_logger_data_p;
};


#define QtUtilsInfoV()    (QtUtilsInfo()<< ::qtutils::Logger::FileAndLineString(__FILE__,__LINE__) << ": ")


}  // namespace qtutils{
