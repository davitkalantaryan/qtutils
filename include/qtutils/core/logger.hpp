//
// file:            logger.hpp
// path:			include/focust/core/logger.hpp
// created on:		2022 Jan 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once


#include <focust/core/focust_internal_header.h>
#include <focust/core/disable_utils_warnings.h>
#include <QDebug>
#include <QMessageLogger>


namespace focust{

class FOCUST_DLL_PRIVATE Logger_p;

#define FocustDebugNV       QMessageLogger(__FILE__, __LINE__, __FUNCTION__).debug
#define FocustDebug()       (FocustDebugNV()<< ::focust::Logger::FileAndLineString(__FILE__,__LINE__) << ": ")
#define FocustDebugV()      (FileLineAndFunctionString()<< ::focust::Logger::FileLineAndFunctionString(__FILE__,__LINE__,__FUNCTION__) << ": ")
#define FocustInfo          QMessageLogger(__FILE__, __LINE__, __FUNCTION__).info
#define FocustWarning       QMessageLogger(__FILE__, __LINE__, __FUNCTION__).warning
#define FocustCritical      QMessageLogger(__FILE__, __LINE__, __FUNCTION__).critical
#define FocustFatal         QMessageLogger(__FILE__, __LINE__, __FUNCTION__).fatal


class FOCUST_EXPORT Logger final
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


#define FocustInfoV()    (FocustInfo()<< ::focust::Logger::FileAndLineString(__FILE__,__LINE__) << ": ")


}  // namespace focust{
