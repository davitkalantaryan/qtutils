//
// file:            settings.hpp
// path:			include/monitor/core/settings.hpp
// created on:		2021 Nov 06
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

//#define FOCUST_EMSCRIPTEN_IS_USED

#include <focust/core/focust_internal_header.h>
#include <focust/core/disable_utils_warnings.h>


#ifdef FOCUST_EMSCRIPTEN_IS_USED
#include <QString>
#include <QVariant>
#include <QSettings>
#else
#include <QSettings>
#endif

namespace focust{


#ifdef FOCUST_EMSCRIPTEN_IS_USED

class FOCUST_DLL_PRIVATE Settings final
{
public:
    bool contains(const FOCUST_QT_NSP QString &key) const;
    FOCUST_QT_NSP QVariant value(const FOCUST_QT_NSP QString &key, const FOCUST_QT_NSP QVariant &defaultValue = FOCUST_QT_NSP QVariant()) const;
    void setValue(const FOCUST_QT_NSP QString &key, const FOCUST_QT_NSP QVariant &value);
	static void setDefaultFormat( FOCUST_QT_NSP QSettings::Format format);
};

#else
typedef FOCUST_QT_NSP QSettings Settings;
#endif  // #ifdef FOCUST_EMSCRIPTEN_IS_USED


}  // namespace focust{
