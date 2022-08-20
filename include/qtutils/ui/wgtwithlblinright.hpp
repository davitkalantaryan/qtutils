//
// file:            wgtwithlblinright.hpp
// path:			include/qtutils/ui/wgtwithlblinright.hpp
// created on:		2022 Aug 20
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_WGTWITHLBLINRIGHT_HPP
#define QTUTILS_INCLUDE_QTUTILS_UI_WGTWITHLBLINRIGHT_HPP


#include <qtutils/qtutils_internal_header.h>
#include <qtutils/ui/sizeapplywindow.hpp>
#include <functional>
#include <qtutils/disable_utils_warnings.h>
#include <QLabel>


namespace qtutils { namespace ui{


template <typename WidgetType>
class QTUTILS_EXPORT WgtWithLblInRight : public ::qtutils::ui::SizeApplyWindow<WidgetType>
{
public:
    template<typename... Targs>
    WgtWithLblInRight(Targs... a_args);
    virtual ~WgtWithLblInRight() override;

    QLabel* labelInRight()const;
    void    SetLabelInRight(QLabel* a_pLabelInRight);
    
protected:
    virtual void ApplyNewSize(const QSize& a_newSize) override;

protected:
    QLabel*     m_pLabelInRight;
};


class QTUTILS_EXPORT Label01 : public QLabel
{
public:
    typedef ::std::function<void(Label01*)> TypeClbk;
    //typedef void (*TypeClbk)(QWidget*,Label01*);

public:
    template<typename... Targs>
    Label01(const TypeClbk& a_lblPushClbk, Targs... a_args);
    virtual ~Label01() override;

protected:
    virtual void mousePressEvent(QMouseEvent* a_event) override;

private:
    const TypeClbk&  m_lblPushClbk;
};


QTUTILS_EXPORT void SetProperParamsToImagedLbl(QLabel* a_pLbl);


}}  // namespace qtutils { namespace ui{


#ifndef QTUTILS_INCLUDE_QTUTILS_UI_WGTWITHLBLINRIGHT_IMPL_HPP
#include "wgtwithlblinright.impl.hpp"
#endif



#endif  // #ifndef QTUTILS_INCLUDE_QTUTILS_UI_WGTWITHLBLINRIGHT_HPP
