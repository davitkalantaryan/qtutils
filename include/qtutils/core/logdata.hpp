//
// file:            logdata.hpp
// path:			include/qtutils/core/logdata.hpp
// created on:		2023 Apr 23
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#pragma once

#include <qtutils/export_symbols.h>
#include <memory>
#include <string>
#include <qtutils/disable_utils_warnings.h>
#include <QMetaType>
#include <QDebug>
#include <QMessageLogContext>
#include <QString>

QTUTILS_CORE_NTDT_NSP_P1


class QTUTILS_EXPORT QtutilsLogDataRaw
{
public:
    QtMsgType           msgType;
    QMessageLogContext  ctx;
    QString             msg;
    ::std::string       categoryStr;

public:
    QtutilsLogDataRaw(const QMessageLogContext& a_ctx);
    QtutilsLogDataRaw(const QMessageLogContext& a_ctx, const ::std::string& a_category);
    QtutilsLogDataRaw(const QtutilsLogDataRaw&)=delete;
    QtutilsLogDataRaw(QtutilsLogDataRaw&&)=delete;
    QtutilsLogDataRaw& operator=(const QtutilsLogDataRaw&)=delete;
    QtutilsLogDataRaw& operator=(QtutilsLogDataRaw&&)=delete;
};

class QTUTILS_EXPORT QtutilsLogData final : public ::std::shared_ptr<QtutilsLogDataRaw>
{
public:
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QtutilsLogData() = default; // qt version 5 needs default constructor for Q_DECLARE_METATYPE
#endif
    QtutilsLogData(QtMsgType a_msgType, const QMessageLogContext& a_ctx, const QString& a_msg);
    QtutilsLogData(QtMsgType a_msgType, const QMessageLogContext& a_ctx, const QString& a_msg, const ::std::string& a_category);
};
Q_DECLARE_METATYPE(QTUTILS_CORE_NTDT_NSP QtutilsLogData)


QTUTILS_CORE_NTDT_NSP_P2
