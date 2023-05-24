//
// file:            qtutils_core_appexiter.hpp
// path:			src/core/qtutils_core_appexiter.hpp
// created on:		2023 May 18
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/core/appexiter.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QMetaObject>
#include <QCoreApplication>



namespace qtutils { namespace core{


AppExiter::~AppExiter()
{
	QMetaObject::invokeMethod(qApp,[](){
		QCoreApplication::quit(); 
	});
}


AppExiterPtr::AppExiterPtr()
    :
      ::std::shared_ptr<AppExiter>(new AppExiter())
{
}


}}  // namespace qtutils { namespace core{
