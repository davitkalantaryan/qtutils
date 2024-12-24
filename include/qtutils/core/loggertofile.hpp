//
// file:            loggertofile.hpp
// path:			include/qtutils/tools/loggertofile.hpp
// created on:		2022 Oct 13
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#include <qtutils/export_symbols.h>
#include <qtutils/core/logger.hpp>
#include <cinternal/disable_compiler_warnings.h>
#include <memory>
#include <qtutils/disable_utils_warnings.h>
#include <QDate>
#include <QFile>
#include <QDir>
#include <cinternal/undisable_compiler_warnings.h>


namespace qtutils { namespace core{ namespace logger{


class CPPUTILS_DLL_PRIVATE LoggerToFile_p;


class QTUTILS_EXPORT ToFile : public Base
{
public:
    ~ToFile() override;
    ToFile();
    void                        RecreateLogFile();
    void                        SetExtraLogger(const Logger::TypeLogger& a_logger, void* a_pOwner=nullptr);
    void                        SetCurrentDate(const QDate& a_newDate);
    void                        SetLogsDir(const QDir& a_logsDir);
    QDate                       currentDate()const;
    Logger::TypeLogger          extraLogger()const;
    void*                       extraLogUserData()const;
    QDir                        logsDir()const;
    QString                     logFilePathCurrentDate()const;
    QString                     logFilePath(const QDate& a_date)const;

protected:
    virtual void LoggerClbk(CinternalLogCategory a_categoryEnm, const char* CPPUTILS_ARG_NN a_categoryStr, char* CPPUTILS_ARG_NN a_log, size_t a_logStrLen) override;

private:
    LoggerToFile_p*const   m_logger_data_p;
};

}}}  //  namespace qtutils { namespace core{ namespace logger{
