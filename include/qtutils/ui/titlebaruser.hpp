//
// file:            titlebaruser.hpp
// path:			include/qtutils/ui/titlebaruser.hpp
// created on:		2022 Aug 16
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_TITLEBARUSER_HPP
#define QTUTILS_INCLUDE_QTUTILS_UI_TITLEBARUSER_HPP


#include <qtutils/export_symbols.h>
#include <qtutils/ui/titlebar.hpp>


namespace qtutils { namespace ui{

template <typename WidgetType>
class TitlebarUser : public SizeApplyWindow<WidgetType>
{
public:
    template<typename... Targs>
    TitlebarUser(Targs... a_args);
    virtual ~TitlebarUser() override;

    void SetTitlebar(TitleBar* a_pTitlebar);
    void ReleaseOwnershipOfTitlebar();
    TitleBar* titlebar()const;
    void SetBelowWidget(QWidget* a_belowWidget);
    void ReleaseOwnershipOfBelowWidget();
    QWidget* belowWidget()const;
    void SetMargins(int a_leftMargin, int a_rightMargin, int a_topMargin, int a_downMargin);
    void SetLeftMargin(int a_leftMargin);
    void SetRightMargin(int a_rightMargin);
    void SetTopMargin(int a_topMargin);
    void SetDownMargin(int a_downMargin);
    int  leftMargin() const;
    int  rightMargin() const;
    int  topMargin() const;
    int  downMargin() const;

protected:
    void ApplyNewSize(const QSize& a_newSize) override;
    
protected:
    TitleBar*      m_pTitleBar;
    QWidget*       m_pBelowWidget;
    int            m_leftMargin, m_rightMargin, m_topMargin, m_downMargin;
};


}}  // namespace qtutils { namespace ui{


#ifndef QTUTILS_INCLUDE_QTUTILS_UI_TITLEBARUSER_IMPL_HPP
#include "titlebaruser.impl.hpp"
#endif



#endif  // #ifndef QTUTILS_INCLUDE_QTUTILS_UI_TITLEBARUSER_HPP
