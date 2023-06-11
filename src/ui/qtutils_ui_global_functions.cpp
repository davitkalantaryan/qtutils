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


QTUTILS_EXPORT void SetProperParamsToImagedLbl(QLabel* a_pLbl)
{
    a_pLbl->setScaledContents( true );
    a_pLbl->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
}


QTUTILS_EXPORT void ShowMessageBox(const QMessageBox::Icon& a_icon, const QString a_title, const QString a_text)
{
#if defined(CPPUTILS_EMSCRIPTEN_IS_USED) || defined(QTUTILS_UI_FORCE_WEB_STYLE)
    ::qtutils::ui::WebDialog<QMessageBox>* pMessageBox = new ::qtutils::ui::WebDialog<QMessageBox>(a_icon,a_title,a_text);
    pMessageBox->open();
#else
    QMessageBox aMessageBox(a_icon,a_title,a_text);
    aMessageBox.exec();
#endif
}


}}  // namespace qtutils { namespace ui{
