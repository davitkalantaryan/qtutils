//
// file:            qtutils_ui_titlebar.hpp
// path:			src/ui/qtutils_ui_titlebar.hpp
// created on:		2022 Aug 16
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/ui/titlebar.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QResizeEvent>
#include <QMouseEvent>


namespace qtutils { namespace ui{

#define QU_TB_CLOSE_BUTTON_SIZE     25


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

static constexpr QSize  s_cCorner = QSize(QU_TB_CLOSE_BUTTON_SIZE,QU_TB_CLOSE_BUTTON_SIZE);
//static constexpr QPoint s_cCornerPt = QPoint(QU_TB_CLOSE_BUTTON_SIZE,QU_TB_CLOSE_BUTTON_SIZE);

TitleBar::~TitleBar()
{
    delete m_pLeftWidget;
}


TitleBar::TitleBar(QWidget* a_parent, QWidget* a_pLeftWidget)
    :
      m_parent(a_parent),
      m_pLeftWidget(a_pLeftWidget),
      m_closeButton(this)
{
    if(!m_pLeftWidget){
        m_pLeftWidget = new QWidget(this);
    }

    m_closeButton.setText("x");
    m_closeButton.setFixedSize(s_cCorner);
    setMinimumSize(s_cCorner);
    resize(width(),QU_TB_CLOSE_BUTTON_SIZE);
    ApplyNewSize(size());

    connect(&m_closeButton,SIGNAL(clicked()),a_parent,SLOT(close()));
}


void TitleBar::SetLeftWidget(QWidget* a_pLeftWidget)
{
    if(a_pLeftWidget && (a_pLeftWidget!=m_pLeftWidget)){
        delete m_pLeftWidget;
        m_pLeftWidget = a_pLeftWidget;
        m_pLeftWidget->setParent(this);
        ApplyNewSize(size());
    }
}


void TitleBar::ReleaseOwnershipOfLeftWidget()
{
    m_pLeftWidget = new QWidget(this);
    ApplyNewSize(size());
}


QWidget* TitleBar::leftWidget()const
{
    return m_pLeftWidget;
}


void TitleBar::setParent(QWidget* a_parent)
{
    QWidget::setParent(a_parent);
    if(a_parent){
        m_parent = a_parent;
    }
}


void TitleBar::mousePressEvent(QMouseEvent* a_event)
{
    if(a_event->button() == Qt::LeftButton){
        m_cursorStartPoint = m_parent->pos() - a_event->globalPos();
    }
    QWidget::mousePressEvent(a_event);
}


void TitleBar::mouseMoveEvent(QMouseEvent* a_event)
{
    if(a_event->buttons() & Qt::LeftButton){
        m_parent->move(m_cursorStartPoint + a_event->globalPos());
    }
    QWidget::mouseMoveEvent(a_event);
}


void TitleBar::resizeEvent(QResizeEvent* a_event)
{
    ApplyNewSize(a_event->size());
    QWidget::resizeEvent(a_event);
}


void TitleBar::ApplyNewSize(const QSize& a_newSize)
{
    m_closeButton.move(a_newSize.width()-QU_TB_CLOSE_BUTTON_SIZE,(a_newSize.width()-QU_TB_CLOSE_BUTTON_SIZE)>1);
}


}}  // namespace qtutils { namespace ui{
