//
// file:            threadobject.hpp
// path:			include/focust/core/threadobject.hpp
// created on:		2022 Jan 12
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once


#include <focust/core/focust_internal_header.h>
#include <focust/core/disable_utils_warnings.h>
#include <QObject>



namespace focust{


class FOCUST_EXPORT ThreadObject : public QObject
{
public:
    virtual ~ThreadObject() override;
    QObject* Sender()const;
};

}  // namespace focust{
