//
// file:			qtutils_ui_pushbutton01.cpp
// path:			src/ui/qtutils_ui_pushbutton01.cpp
// created on:		2022 Oct 20
// created by:		Davit Kalantaryan (davit.kalantaryan1980@gmail.com)
//


#include <qtutils/ui/pushbutton01.hpp>


namespace qtutils{ namespace ui{


void PushButton01::mousePressEvent(QMouseEvent *)
{
}


void PushButton01::mouseReleaseEvent(QMouseEvent *)
{
    emit MouseRelesedSignal();
}


}}  //  namespace qtutils{ namespace ui{
