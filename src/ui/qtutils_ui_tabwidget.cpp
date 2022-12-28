//
// file:            qtutils_ui_tabwidget.cpp
// path:			src/ui/qtutils_ui_tabwidget.cpp
// created on:		2022 Aug 16
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/ui/tabwidget.hpp>
#include <assert.h>
#include <qtutils/disable_utils_warnings.h>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>


namespace qtutils { namespace ui{

#define DEFAULT_TAB_WIDTH_SIZE      128
#define DEFAULT_TAB_HEIGHT_SIZE     30
#define TABWIDGET_HEIGH_AFTER_TAB   5


//struct STabData{
//    Tab*     pTab;
//    QWidget* pWidget;
//    // maybe QIcon* , etc.
//};
//
//struct STabHash{
//    size_t operator()(const STabData& a_dt) const{
//        return reinterpret_cast<size_t>(a_dt.pTab);
//    }
//};
//struct STabEq{
//    bool operator()(const STabData& a_lhs, const STabData& a_rhs) const{
//        return a_lhs.pTab == a_rhs.pTab;
//    }
//};


/*/////////////////////////////////////////////////////////////////////////////////////////////////////////*/

TabScene::~TabScene()
{
}


void TabScene::resizeEvent(QResizeEvent* a_event)
{
    QWidget::resizeEvent(a_event);
    if(m_pTabActiveWidget){
        m_pTabActiveWidget->resize(a_event->size());
    }
}


/*/////////////////////////////////////////////////////////////////////////////////////////////////////////*/

TabBar::TabBar(TabScene* a_sceneWidget, QWidget* a_parent)
    :
      QWidget(a_parent),
      m_sceneWidget(a_sceneWidget)
{
    m_tabsWidth = DEFAULT_TAB_WIDTH_SIZE;
    m_currentTab = -1;
    resize(width(),DEFAULT_TAB_HEIGHT_SIZE);
}


TabBar::~TabBar()
{
    TypeHash::iterator iter;
    const size_t cunSize(m_tabs.size());

    for(size_t i(0); i<cunSize;++i){
        iter = m_tabs[i];
        //delete iter->second.pWidget; // todo: we should check if this is deleted
        delete iter->first.pTab;
    }

}


Tab* TabBar::addTabRaw(QWidget* a_page, Tab* a_pTab)
{
    //if((!a_page)||(!a_pTab)){return -1;}
    assert(a_page&&a_pTab);

    STabData tabData;
    const int cnRet(static_cast<int>(m_tabs.size()));

    a_pTab->m_index = cnRet;
    a_pTab->setParent(this);

    if(m_styleSheetForSelected.size()>0){
        a_pTab->m_styleSheetForSelected = m_styleSheetForSelected;
    }
    else{
        a_pTab->m_styleSheetForSelected = a_pTab->styleSheet();
    }

    if(m_styleSheetNonSelected.size()>0){
        a_pTab->m_styleSheetNonSelected = m_styleSheetNonSelected;
    }
    else{
        a_pTab->m_styleSheetNonSelected = a_pTab->styleSheet();
    }

    tabData.pWidget = a_page;
    tabData.pTab = a_pTab;
    const int cnHeigth(height());
    tabData.pTab->setFixedSize(m_tabsWidth,cnHeigth);
    tabData.pTab->setStyleSheet(a_pTab->m_styleSheetNonSelected);
    m_tabs.AddEntryEvenIfExistsC(tabData);

    if(m_currentTab<0){
        setCurrentIndexRaw(cnRet);
    }
    else{
        a_page->hide();
    }

    OrderAllTabs();

    return a_pTab;
}


void TabBar::setCurrentIndex(int a_index)
{
    const int cnSize(int(m_tabs.size()));
    if(a_index<cnSize){
        setCurrentIndexRaw(a_index);
    }
}


void TabBar::OrderAllTabs()
{
    TypeHash::iterator iter;
    const size_t tabsCount = m_tabs.size();
    int moveToX = 0;

    for(size_t i(0); i<tabsCount;++i,moveToX+=m_tabsWidth){
        iter = m_tabs[i];
        iter->first.pTab->move(moveToX,0);
    }
}


void TabBar::setCurrentIndexRaw(int a_index)
{
    if(a_index!=m_currentTab){
        if(m_currentTab>=0){
            TypeHash::iterator iterOld = m_tabs[size_t(m_currentTab)];
            iterOld->first.pWidget->setParent(nullptr);
            iterOld->first.pWidget->hide();
            iterOld->first.pTab->m_isSelected = 0;
            iterOld->first.pTab->setStyleSheet(iterOld->first.pTab->m_styleSheetNonSelected);
            iterOld->first.pTab->update();
        }
        m_currentTab = a_index;
        if(a_index>=0){
            TypeHash::iterator iter = m_tabs[size_t(m_currentTab)];
            m_sceneWidget->m_pTabActiveWidget = iter->first.pWidget;
            iter->first.pWidget->setParent(m_sceneWidget);
            iter->first.pWidget->show();
            iter->first.pWidget->resize(m_sceneWidget->size());
            iter->first.pWidget->move(0,0);
            iter->first.pTab->m_isSelected = 1;
            iter->first.pTab->setStyleSheet(iter->first.pTab->m_styleSheetForSelected);
            iter->first.pTab->update();
        }
        else{
            m_sceneWidget->m_pTabActiveWidget = nullptr;
        }
    }
}


int TabBar::currentIndex()const
{
    return m_currentTab;
}


const QString& TabBar::styleSheetForSelected()const
{
    return m_styleSheetForSelected;
}


const QString& TabBar::styleSheetNonSelected()const
{
    return m_styleSheetNonSelected;
}


void TabBar::SetStyleSheetForSelected(const QString& a_newVal)
{
    m_styleSheetForSelected = a_newVal;
}


void TabBar::SetStyleSheetNonSelected(const QString& a_newVal)
{
    m_styleSheetNonSelected = a_newVal;
}


int TabBar::tabsWidth()const
{
    return m_tabsWidth;
}


void TabBar::SetTabsWidth(int a_tabsWidth)
{
    m_tabsWidth = a_tabsWidth;
    OrderAllTabs();
}


//

Tab::~Tab()
{
}


void Tab::mousePressEvent(QMouseEvent* a_event)
{
    QLabel::mousePressEvent(a_event);
    const Qt::MouseButton btn = a_event->button();

    switch(btn){
    case Qt::LeftButton:{
        TabBar* pParent = static_cast<TabBar*>(parentWidget());
        pParent->setCurrentIndexRaw(m_index);
    }break;
    default:
        break;
    }  // switch(btn){
}


int Tab::index()const
{
    return m_index;
}


int Tab::isSelected()const
{
    return m_isSelected;
}


const QString& Tab::styleSheetForSelected()const
{
    return m_styleSheetForSelected;
}


const QString& Tab::styleSheetNonSelected()const
{
    return m_styleSheetNonSelected;
}


void Tab::SetStyleSheetForSelected(const QString& a_newVal)
{
    m_styleSheetForSelected = a_newVal;
}


void Tab::SetStyleSheetNonSelected(const QString& a_newVal)
{
    m_styleSheetNonSelected = a_newVal;
}


//



//





}} // namespace qtutils { namespace ui{namespace qtutils { namespace ui{
