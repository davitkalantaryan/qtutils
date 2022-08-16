//
// file:            titlebar.hpp
// path:			include/qtutils/ui/titlebar.hpp
// created on:		2022 Aug 16
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#include <qtutils/qtutils_internal_header.h>
#include <qtutils/ui/sizeapplywindow.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QWidget>
#include <QLabel>
#include <QSize>
#include <QPoint>


namespace qtutils { namespace ui{

#define QU_TB_CLOSE_BUTTON_SIZE     25


class CPPUTILS_DLL_PRIVATE CloseOrMnmWdg;


class QTUTILS_EXPORT  TitleBar : public SizeApplyWindow<QWidget>
{
public:
    virtual ~TitleBar() override;
    TitleBar(QWidget* parent, QWidget* a_lefWidget = nullptr);

    void SetLeftWidget(QWidget* a_lefWidget);
    void ReleaseOwnershipOfLeftWidget();
    QWidget* leftWidget()const;
    void setParent(QWidget *parent);

protected:
    virtual void    mousePressEvent(QMouseEvent *event) override;
    virtual void    mouseMoveEvent(QMouseEvent *event) override;

private:
    void ApplyNewSize(const QSize& a_newSize) override;

private:
     QWidget*               m_parent;
     QWidget*               m_pLeftWidget;
     QPoint                 m_cursorStartPoint;
     CloseOrMnmWdg*const    m_pCloseButton;
     CloseOrMnmWdg*const    m_pMnmzButton;

     friend class CloseOrMnmWdg;
};


}}  // namespace qtutils { namespace ui{
