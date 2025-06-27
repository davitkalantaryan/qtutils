//
// file:            invokeblocked.cpp
// path:			src/core/qtutils_core_invokeblocked.cpp
// created on:		2022 Apr 30
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/core/invokeblocked.hpp>
#include <cinternal/disable_compiler_warnings.h>
#include <qtutils/disable_utils_warnings.h>
#include <QMetaObject>
#include <QThread>
#include <cinternal/undisable_compiler_warnings.h>

namespace qtutils {

QTUTILS_EXPORT bool invokeMethodBlocked(QObject* a_pObj, const ::std::function<void(void)>& a_fnc)
{
    if((!a_pObj) || (a_pObj->thread() == QThread::currentThread())){
        a_fnc();
    }
    else{
        return QMetaObject::invokeMethod(a_pObj,[=](){
            a_fnc();
        },Qt::BlockingQueuedConnection);
    }

    return true;
}


}  // namespace qtutils {
