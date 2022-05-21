//
// file:            invokeblocked.impl.hpp
// path:			include/qtutils/core/invokeblocked.impl.hpp
// created on:		2022 Apr 30
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#ifndef QTUTILS_INCLUDE_QTUTILS_CORE_INVOKEBLOCKED_IMPL_HPP
#define QTUTILS_INCLUDE_QTUTILS_CORE_INVOKEBLOCKED_IMPL_HPP

#ifndef QTUTILS_INCLUDE_QTUTILS_CORE_INVOKEBLOCKED_HPP
#include "invokeblocked.hpp"
#endif
#include <qtutils/disable_utils_warnings.h>
#include <QMetaObject>
#include <QThread>

namespace qtutils {


template<typename... Targs>
bool invokeMethodBlocked(QObject* a_pObj, const ::std::function<void(Targs...)>& a_fnc, Targs... a_args)
{
    if(a_pObj->thread() == QThread::currentThread()){
        a_fnc(a_args...);
    }
    else{
        return QMetaObject::invokeMethod(a_pObj,[=](){
            a_fnc(a_args...);
        },Qt::BlockingQueuedConnection);
    }

    return true;
}


}  // namespace qtutils {



#endif  // #ifndef QTUTILS_INCLUDE_QTUTILS_CORE_INVOKEBLOCKED_IMPL_HPP
