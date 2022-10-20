//
// file:			pushbutton01.hpp
// path:			include/ui/pushbutton01.hpp
// created on:		2022 Oct 20
// created by:		Davit Kalantaryan (davit.kalantaryan1980@gmail.com)
//

#pragma once

#include <qtutils/export_symbols.h>
#include <qtutils/disable_utils_warnings.h>
#include <QPushButton>


namespace qtutils{ namespace ui{


class QTUTILS_EXPORT PushButton01 : public QPushButton
{
    Q_OBJECT

public:
    using QPushButton::QPushButton;

private:
    void	mousePressEvent(QMouseEvent *) override final;
    void	mouseReleaseEvent(QMouseEvent *) override final;

private:
signals:
    void    MouseRelesedSignal();
};


}}  //  namespace qtutils{ namespace ui{
