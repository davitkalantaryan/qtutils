//
// file:			logwnd.hpp
// path:			include/ui/logwnd.hpp
// created on:		2022 Oct 13
// created by:		Davit Kalantaryan (davit.kalantaryan1980@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_LOGWND_HPP
#define QTUTILS_INCLUDE_QTUTILS_UI_LOGWND_HPP

#include <qtutils/export_symbols.h>
#include <qtutils/core/logger.hpp>
#include <qtutils/ui/sizeapplywindow.hpp>
#include <cinternal/disable_compiler_warnings.h>
#include <qtutils/disable_utils_warnings.h>
#include <QWidget>
#include <QColor>
#include <QMessageLogContext>
#include <cinternal/undisable_compiler_warnings.h>


namespace qtutils { namespace ui{ namespace logger{


class CPPUTILS_DLL_PRIVATE LogWnd_p;

QTUTILS_UI_EXPORT void QtutilsUiLogwndInitialize(void);
QTUTILS_UI_EXPORT void QtutilsUiLogwndCleanup(void);


class QTUTILS_EXPORT LogWnd : public ::qtutils::ui::SizeApplyWindow<QWidget>
{
    Q_OBJECT

public:
    virtual ~LogWnd() override;
    template<typename... Targs>
    LogWnd(Targs... a_args);
    LogWnd(const LogWnd&) = delete;
    LogWnd(LogWnd&&) = delete;
    LogWnd& operator=(const LogWnd&) = delete;
    LogWnd& operator=(LogWnd&&) = delete;

    void InitSettings();

    static const QColor* defaultColors();
    static void SetDefaultColors(const QColor* a_newColors, size_t a_count, size_t a_offset=0);
    static void SetDefaultColor(const core::logger::LogTypes& a_type, const QColor& a_newColor);
    void SetCategoryColors(const QString& a_categoryName, const QColor* a_newColors, size_t a_count, size_t a_offset=0);
    void SetCategoryColors(const QString& a_categoryName, const core::logger::LogTypes& a_type, const QColor& a_newColor);
    void AddLogCategory(const QString& a_categoryName, bool a_defaultEnable);
    void RemoveCategory(const QString& a_categoryName);
    void EnableCategoryType(const QString& a_categoryName, const core::logger::LogTypes& a_type);
    void DisableCategoryType(const QString& a_categoryName, const core::logger::LogTypes& a_type);
    bool isEnabledCategoryType(const QString& a_categoryName, const core::logger::LogTypes& a_type);
    void ClearAllCategories();
    void SetMaxNumberOfLogs(size_t a_logsCount);

protected:
    virtual void showEvent(QShowEvent *event) override;

private:
    void ApplyNewSize(const QSize& a_newSize) override;
    static LogWnd_p* ConstructLogger(LogWnd* a_pLogger);

private:
signals:
    void CategoryTypeEnabledSignal(qtutils_core_logger_CategoryNoty);
    void CategoryTypeDisabledSignal(qtutils_core_logger_CategoryNoty);
    void NewLogAvailableSignal(qtutils_core_logger_LoggerData a_newLogData);

private:
    LogWnd_p*const   m_logwnd_data_p;

    friend class LogWnd_p;
};


#define QTUTILS_UI_LOGWND_TYPE_TO_INDEX_RAW(_int,_type) static_cast<_int>(_type)
#define QTUTILS_UI_LOGWND_TYPE_TO_INDEX(_type)          QTUTILS_UI_LOGWND_TYPE_TO_INDEX_RAW(size_t,_type)
#define QTUTILS_UI_LOGWND_DEFAULT_MAX_NUMBER_OF_LOGS    1000


}}}  //  namespace qtutils { namespace ui{ namespace logger{


#ifndef QTUTILS_INCLUDE_QTUTILS_UI_LOGWND_IMPL_HPP
#include "logwnd.impl.hpp"
#endif


#endif  //  #ifndef QTUTILS_INCLUDE_QTUTILS_UI_LOGWND_HPP
