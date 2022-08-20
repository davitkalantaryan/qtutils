//
// file:            qtutils_ui_wgtwithlblinright.hpp
// path:			src/ui/qtutils_ui_editwithlabelinright.hpp
// created on:		2022 Aug 20
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/ui/wgtwithlblinright.hpp>
#include <qtutils/core/logger.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QLabel>
#include <QResizeEvent>
#include <QMouseEvent>


namespace qtutils { namespace ui{

//

Label01::~Label01()
{
}


void Label01::mousePressEvent(QMouseEvent* a_event)
{
    QLabel::mousePressEvent(a_event);
    switch(a_event->button()){
    case Qt::LeftButton:
        //m_lblPushClbk(dynamic_cast<EditWithLabelInRight*>(parentWidget()),this);
        m_lblPushClbk(this);
        QtUtilsDebug()<<" ";
        break;
    default:
        break;
    }
}


QTUTILS_EXPORT void SetProperParamsToImagedLbl(QLabel* a_pLbl)
{
    a_pLbl->setScaledContents( true );
    a_pLbl->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
}


}}  // namespace qtutils { namespace ui{
