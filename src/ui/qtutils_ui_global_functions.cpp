//
// file:            qtutils_ui_global_functions.hpp
// path:			src/ui/qtutils_ui_global_functions.hpp
// created on:		2022 Jan 19
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/ui/global_functions.hpp>
//#include <qtutils/disable_utils_warnings.h>


namespace qtutils { namespace ui{


QTUTILS_EXPORT bool isSameWidgetOrChild(QWidget* a_pPossibleParent, QWidget* a_pPossibleChild)
{
    QWidget* pWidgetToCmp = a_pPossibleChild;
    while(pWidgetToCmp){
        if(pWidgetToCmp==a_pPossibleParent){
            return true;
        }
        pWidgetToCmp = pWidgetToCmp->parentWidget();
    }
    
    return false;
}


QTUTILS_EXPORT void TakeFrameOfWindow(QWidget* a_pWindow)
{
	const Qt::WindowFlags spinnerFlags = a_pWindow->windowFlags();
	a_pWindow->setWindowFlags(spinnerFlags|Qt::FramelessWindowHint);
}


QTUTILS_EXPORT void AssignFrameToWindow(QWidget* a_pWindow)
{
	const Qt::WindowFlags spinnerFlags = a_pWindow->windowFlags();
	a_pWindow->setWindowFlags(spinnerFlags&(~Qt::FramelessWindowHint));
}


}}  // namespace qtutils { namespace ui{
