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
#include <QFont>


namespace qtutils { namespace ui{

/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

static constexpr QSize  s_cCorner = QSize(QU_TB_CLOSE_BUTTON_SIZE,QU_TB_CLOSE_BUTTON_SIZE);
static constexpr QSize  s_cCornerDubl = QSize(2*QU_TB_CLOSE_BUTTON_SIZE,QU_TB_CLOSE_BUTTON_SIZE);
//static constexpr QPoint s_cCornerPt = QPoint(QU_TB_CLOSE_BUTTON_SIZE,QU_TB_CLOSE_BUTTON_SIZE);


class CPPUTILS_DLL_PRIVATE CloseOrMnmWdg final : public QLabel
{
public:
    enum class Type{None,Close,Mnmz};

public:
    CloseOrMnmWdg(TitleBar* a_pParent, const Type& a_type);

private:
    void    mouseReleaseEvent(QMouseEvent* event) override;
    bool    event(QEvent* a_event) override;

private:
    TitleBar*const  m_pParent;
    const Type      m_type;
    QString         m_styleSheetStr;
};


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/


TitleBar::~TitleBar()
{
    delete m_pMnmzButton;
    delete m_pCloseButton;
    delete m_pLeftWidget;
}


TitleBar::TitleBar(QWidget* a_parent, QWidget* a_pLeftWidget)
    :
      m_parent(a_parent),
      m_pLeftWidget(a_pLeftWidget),
      m_pCloseButton(new CloseOrMnmWdg(this,CloseOrMnmWdg::Type::Close)),
      m_pMnmzButton(new CloseOrMnmWdg(this,CloseOrMnmWdg::Type::Mnmz))
{
    if(!m_pLeftWidget){
        m_pLeftWidget = new QWidget(this);
    }

    wchar_t vcClose[2];
    //vcClose[0] = char(158);
    vcClose[0] = 215;
    vcClose[1] = 0;
    m_pCloseButton->setText(QString::fromWCharArray(vcClose));
    m_pCloseButton->setFixedSize(s_cCorner);
    m_pMnmzButton->setText("-");
    m_pMnmzButton->setFixedSize(s_cCorner);
    setMinimumSize(s_cCornerDubl);
    resize(width(),QU_TB_CLOSE_BUTTON_SIZE);
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
    SizeApplyWindow<QWidget>::mousePressEvent(a_event);
}


void TitleBar::mouseMoveEvent(QMouseEvent* a_event)
{
    if(a_event->buttons() & Qt::LeftButton){
        m_parent->move(m_cursorStartPoint + a_event->globalPos());
    }
    SizeApplyWindow<QWidget>::mouseMoveEvent(a_event);
}


void TitleBar::ApplyNewSize(const QSize& a_newSize)
{
    int nMovePosX = a_newSize.width()-QU_TB_CLOSE_BUTTON_SIZE;
    const int nMovePosY = (a_newSize.height()-QU_TB_CLOSE_BUTTON_SIZE)>1;
    m_pCloseButton->move(nMovePosX,nMovePosY);
    m_pMnmzButton->move(nMovePosX-QU_TB_CLOSE_BUTTON_SIZE,nMovePosY);
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

CloseOrMnmWdg::CloseOrMnmWdg(TitleBar* a_pParent, const Type& a_type)
    :
      QLabel(a_pParent),
      m_pParent(a_pParent),
      m_type(a_type)
{
    QFont aFont = font();
    switch(m_type){
    case Type::Mnmz:
        aFont.setPointSize(24);
        break;
    default:
        aFont.setPointSize(18);
        break;
    } // switch(m_type){
    setFont(aFont);
    setAlignment(Qt::AlignCenter);
    setAttribute(Qt::WA_Hover, true);
}


void CloseOrMnmWdg::mouseReleaseEvent(QMouseEvent* a_event)
{
    if(a_event->button() == Qt::LeftButton){
        switch(m_type){
        case Type::Close:
            m_pParent->m_parent->close();
            break;
        case Type::Mnmz:
            m_pParent->m_parent->showMinimized();
            break;
        default:
            break;
        } // switch(m_type){
    }
    QWidget::mouseMoveEvent(a_event);
}


bool CloseOrMnmWdg::event(QEvent* a_event)
{
    switch(a_event->type())
    {
    case QEvent::HoverEnter:
        m_styleSheetStr = styleSheet();
        setStyleSheet("Background-color: rgb(0,255,155);");
        return true;
    case QEvent::HoverLeave:
        setStyleSheet(m_styleSheetStr);
        return true;
    default:
        break;
    }
    return QWidget::event(a_event);
}


}}  // namespace qtutils { namespace ui{
