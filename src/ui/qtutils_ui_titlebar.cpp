//
// file:            qtutils_ui_titlebar.hpp
// path:			src/ui/qtutils_ui_titlebar.hpp
// created on:		2022 Aug 16
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/ui/titlebar.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <functional>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QFont>


namespace qtutils { namespace ui{

/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

static constexpr QSize  s_cCorner = QSize(QU_TB_CLOSE_BUTTON_SIZE,QU_TB_CLOSE_BUTTON_SIZE);
//static constexpr QSize  s_cCornerDubl = QSize(2*QU_TB_CLOSE_BUTTON_SIZE,QU_TB_CLOSE_BUTTON_SIZE);
//static constexpr QPoint s_cCornerPt = QPoint(QU_TB_CLOSE_BUTTON_SIZE,QU_TB_CLOSE_BUTTON_SIZE);


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/


TitleBar::~TitleBar()
{
    ::std::list<CloseOrMnmWdg*>::const_iterator citer = m_rsButtons.cbegin();
    const ::std::list<CloseOrMnmWdg*>::const_iterator citerEnd = m_rsButtons.cend();
    for(;citer!=citerEnd;++citer){
        delete *citer;
    }
    delete m_pLeftWidget;
}


TitleBar::TitleBar(QWidget* a_parent, QWidget* a_pLeftWidget)
    :
      m_parent(a_parent),
      m_pLeftWidget(a_pLeftWidget)
{
    if(!m_pLeftWidget){
        m_pLeftWidget = new QWidget(this);
    }

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


void TitleBar::AddRightCornerButton(CloseOrMnmWdg* a_pBtn)
{
    a_pBtn->setMinimumSize(s_cCorner);
    m_rsButtons.push_back(a_pBtn);
    ApplyNewSize(size());
}


void TitleBar::AddRightCornerStdButton(const CloseOrMnmWdg::StdType& a_stdType)
{
    AddRightCornerButton(new CloseOrMnmWdg(this,a_stdType));
}


QWidget* TitleBar::tbParent()const
{
    return m_parent;
}


void TitleBar::mousePressEvent(QMouseEvent* a_event)
{
    if(a_event->button() == Qt::LeftButton){
#ifdef QTUTILS_TITLEBAR_SEARCH_TOP_MOST
        QWidget *pParent = m_parent, *pParentTmp = m_parent->parentWidget();
        while(pParentTmp){
            pParent = pParentTmp;
            pParentTmp = pParent->parentWidget();
        }
        m_cursorStartPoint = pParent->pos() - a_event->globalPos();
#else
        m_cursorStartPoint = m_parent->pos() - a_event->globalPos();
#endif
    }
    SizeApplyWindow<QWidget>::mousePressEvent(a_event);
}


void TitleBar::mouseMoveEvent(QMouseEvent* a_event)
{
    if(a_event->buttons() & Qt::LeftButton){
#ifdef QTUTILS_TITLEBAR_SEARCH_TOP_MOST
        QWidget *pParent = m_parent, *pParentTmp = m_parent->parentWidget();
        while(pParentTmp){
            pParent = pParentTmp;
            pParentTmp = pParent->parentWidget();
        }
        pParent->move(m_cursorStartPoint + a_event->globalPos());
#else
        m_parent->move(m_cursorStartPoint + a_event->globalPos());
#endif
    }
    SizeApplyWindow<QWidget>::mouseMoveEvent(a_event);
}


void TitleBar::ApplyNewSize(const QSize& a_newSize)
{
    const int cnSizeY = a_newSize.height();
    const int cnSizeX = a_newSize.width();
    int nUnit,nMovePosY,nMovePosX;

    if(cnSizeX<cnSizeY){
        nUnit = cnSizeX;
        nMovePosY = (cnSizeY-cnSizeX)/2;
    }
    else{
        nUnit = cnSizeY;
        nMovePosY = 0;
    }
    nMovePosX = cnSizeX;

    ::std::list<CloseOrMnmWdg*>::const_reverse_iterator criter = m_rsButtons.crbegin();
    const ::std::list<CloseOrMnmWdg*>::const_reverse_iterator criterEnd = m_rsButtons.rend();

    for(;criter!=criterEnd;++criter){
        nMovePosX-=nUnit;
        (*criter)->setFixedSize(nUnit,nUnit);
        (*criter)->move(nMovePosX,nMovePosY);
    }

    m_pLeftWidget->setFixedSize(nMovePosX,cnSizeY);
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

static void StaticCallbackFunction(TitleBar*,CloseOrMnmWdg*){}

#define QTUTILS_TABBAR_MAX_SYMBOL       0x1f5d6
#define QTUTILS_TABBAR_NO_MAX_SYMBOL    0x1f5d7

static void StaticCallbackForStdTypes(TitleBar* a_pTitleBar, CloseOrMnmWdg* a_pBtn)
{
    QWidget* pParent = a_pTitleBar->tbParent();
#ifdef QTUTILS_TITLEBAR_SEARCH_TOP_MOST
    QWidget* pParentTmp = pParent->parentWidget();
    while(pParentTmp){
        pParent = pParentTmp;
        pParentTmp = pParent->parentWidget();
    }
#endif

    switch(a_pBtn->m_stdType){
    case CloseOrMnmWdg::StdType::Close:
        pParent->close();
        break;
    case CloseOrMnmWdg::StdType::Mnmz:
        pParent->showMinimized();
        break;
    default:
        if(pParent->isMaximized()){
            const char32_t vcTxt[2] = {QTUTILS_TABBAR_MAX_SYMBOL,0};
            pParent->showNormal();
            a_pBtn->setText(QString::fromUcs4(vcTxt));
        }
        else{
            const char32_t vcTxt[2] = {QTUTILS_TABBAR_NO_MAX_SYMBOL,0};
            pParent->showMaximized();
            a_pBtn->setText(QString::fromUcs4(vcTxt));
        }
        break;
    } // switch(m_type){
}


CloseOrMnmWdg::~CloseOrMnmWdg()
{
}


CloseOrMnmWdg::CloseOrMnmWdg(const QString& a_styleSheetWhenHover,TitleBar* a_pParent, const TypeClbk& a_clbk)
    :
      QLabel(a_pParent),
      m_pParent(a_pParent),
      m_clbk(a_clbk?a_clbk:(&StaticCallbackFunction)),
      m_stdType(StdType::None),
      m_styleSheetWhenHover(a_styleSheetWhenHover)
{
    QFont aFont = font();
    aFont.setPixelSize(23);
    setFont(aFont);
    setAlignment(Qt::AlignCenter);
    setAttribute(Qt::WA_Hover, true);
}


CloseOrMnmWdg::CloseOrMnmWdg(TitleBar* a_pParent, const StdType& a_stdType)
    :
      QLabel(a_pParent),
      m_pParent(a_pParent),
      m_clbk(&StaticCallbackForStdTypes),
      m_stdType(a_stdType),
      m_styleSheetWhenHover("Background-color: rgb(0,255,155);")
{
    char32_t vcTxt[2];
    QFont aFont = font();

    switch(m_stdType){
    case StdType::Close:
        vcTxt[0] = 215;
        aFont.setPixelSize(20);
        break;
    case StdType::Mnmz:
        vcTxt[0] = '-';
        aFont.setPixelSize(26);
        break;
    default:
        vcTxt[0] = a_pParent->tbParent()->isMaximized()?QTUTILS_TABBAR_NO_MAX_SYMBOL:QTUTILS_TABBAR_MAX_SYMBOL;
        aFont.setPixelSize(14);
        break;
    } // switch(m_type){

    setFont(aFont);
    vcTxt[1] = 0;
    setText(QString::fromUcs4(vcTxt));

    setAlignment(Qt::AlignCenter);
    setAttribute(Qt::WA_Hover, true);

}


void CloseOrMnmWdg::mouseReleaseEvent(QMouseEvent* a_event)
{
    m_clbk(m_pParent,this);
    QWidget::mouseMoveEvent(a_event);
}


bool CloseOrMnmWdg::event(QEvent* a_event)
{
    switch(a_event->type())
    {
    case QEvent::HoverEnter:
        m_styleSheetBackp = styleSheet();
        setStyleSheet(m_styleSheetWhenHover/*"Background-color: rgb(0,255,155);"*/);
        return true;
    case QEvent::HoverLeave:
        setStyleSheet(m_styleSheetBackp);
        return true;
    default:
        break;
    }
    return QWidget::event(a_event);
}


}}  // namespace qtutils { namespace ui{
