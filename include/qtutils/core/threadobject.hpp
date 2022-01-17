//
// file:            threadobject.hpp
// path:			include/qtutils/core/threadobject.hpp
// created on:		2022 Jan 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once


#include <qtutils/qtutils_internal_header.h>
#include <qtutils/disable_utils_warnings.h>
#include <QObject>



namespace qtutils{


class QTUTILS_EXPORT ThreadObject : public QObject
{
public:
    virtual ~ThreadObject() override;
    QObject* Sender()const;
};

}  // namespace qtutils{
