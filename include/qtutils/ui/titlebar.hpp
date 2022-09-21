//
// file:            titlebar.hpp
// path:			include/qtutils/ui/titlebar.hpp
// created on:		2022 Aug 16
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#include <qtutils/export_symbols.h>
#include <qtutils/sizes.h>
#include <qtutils/ui/sizeapplywindow.hpp>
#include <list>
#include <qtutils/disable_utils_warnings.h>
#include <QWidget>
#include <QLabel>
#include <QSize>
#include <QPoint>


namespace qtutils { namespace ui{

#define QU_TB_CLOSE_BUTTON_SIZE     QTUTILS_TITLEBAR_DEFAULT_HEIGHT

class QTUTILS_EXPORT  TitleBar;


class QTUTILS_EXPORT CloseOrMnmWdg : public QLabel
{
public:
    typedef ::std::function<void(TitleBar*,CloseOrMnmWdg*)> TypeClbk;
    enum class StdType{None,Close,Mnmz,Maxmz};

public:
    virtual ~CloseOrMnmWdg() override;
    CloseOrMnmWdg(const QString& a_styleSheetWhenHover, TitleBar* a_pParent, const TypeClbk& a_clbk);
    CloseOrMnmWdg(TitleBar* a_pParent, const StdType& a_stdType);
    CloseOrMnmWdg(const CloseOrMnmWdg&)=delete;
    CloseOrMnmWdg& operator=(const CloseOrMnmWdg&)=delete;

protected:
    void    mouseReleaseEvent(QMouseEvent* event) override;
    bool    event(QEvent* a_event) override;

public:
    TitleBar*const  m_pParent;
    const TypeClbk  m_clbk;
    const StdType   m_stdType;
    const QString   m_styleSheetWhenHover;

private:
    QString         m_styleSheetBackp;
};


class QTUTILS_EXPORT  TitleBar : public SizeApplyWindow<QWidget>
{
public:
    virtual ~TitleBar() override;
    TitleBar(QWidget* parent, QWidget* a_lefWidget = nullptr);

    void SetLeftWidget(QWidget* a_lefWidget);
    void ReleaseOwnershipOfLeftWidget();
    QWidget* leftWidget()const;
    void setParent(QWidget *parent);
    void AddRightCornerButton(CloseOrMnmWdg* a_pBtn);
    void AddRightCornerStdButton(const CloseOrMnmWdg::StdType& a_stdType);
    QWidget* tbParent()const;

protected:
    virtual void    mousePressEvent(QMouseEvent *event) override;
    virtual void    mouseMoveEvent(QMouseEvent *event) override;

private:
    void ApplyNewSize(const QSize& a_newSize) override;

private:
     QWidget*                           m_parent;
     QWidget*                           m_pLeftWidget;
     QPoint                             m_cursorStartPoint;
     ::std::list<CloseOrMnmWdg*>        m_rsButtons;

};


}}  // namespace qtutils { namespace ui{
