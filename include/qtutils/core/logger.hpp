//
// file:            logger.hpp
// path:			include/focust/core/logger.hpp
// created on:		2022 Jan 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once


#include <qtutils/qtutils_internal_header.h>
#include <qtutils/disable_utils_warnings.h>
#include <QDebug>
#include <QMessageLogger>


namespace qtutils { 

class CPPUTILS_DLL_PRIVATE Logger_p;

#define QtUtilsDebugNV       QMessageLogger(__FILE__, __LINE__, __FUNCTION__).debug
#define QtUtilsDebug()       (QtUtilsDebugNV()<< ::qtutils::Logger::FileAndLineString(__FILE__,__LINE__) << ": ")
#define QtUtilsDebugV()      (QtUtilsDebugNV()<< ::qtutils::Logger::FileLineAndFunctionString(__FILE__,__LINE__,__FUNCTION__) << ": ")
#define QtUtilsInfo          QMessageLogger(__FILE__, __LINE__, __FUNCTION__).info
#define QtUtilsWarning       QMessageLogger(__FILE__, __LINE__, __FUNCTION__).warning
#define QtUtilsCritical      QMessageLogger(__FILE__, __LINE__, __FUNCTION__).critical
#define QtUtilsFatal         QMessageLogger(__FILE__, __LINE__, __FUNCTION__).fatal


class QTUTILS_EXPORT Logger final
{
public:
    Logger();
    Logger(const Logger&)=delete;
    Logger(Logger&&)=delete;
    ~Logger();
    Logger& operator=(const Logger&)=delete;
    Logger& operator=(Logger&&)=delete;

    QtMessageHandler SetNewLogger(const QtMessageHandler& a_logger);
    static QString  FileAndLineString(const char* a_fileName, int a_line);
    static QString  FileLineAndFunctionString(const char* a_fileName, int a_line, const char* a_cpcFunction);

private:
    Logger_p*const      m_logger_data_p;
};


#define QtUtilsInfoV()    (QtUtilsInfo()<< ::qtutils::Logger::FileAndLineString(__FILE__,__LINE__) << ": ")


}  // namespace qtutils{
