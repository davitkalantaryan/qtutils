//
// repo:            qtutils
// file:            qtutils_ui_widgetscontainer.cpp
// path:			src/ui/qtutils_ui_widgetscontainer.cpp
// created on:		2023 Jul 02
// created by:		Davit Kalantaryan (davit@focust.ai)
//

#include <qtutils/ui/widgetscontainerbasequ.hpp>
#include <qtutils/core/settings.hpp>
#include <qtutils/core/logger.hpp>
#include <qtutils/ui/global_functions.hpp>
#include <cinternal/flagshelper02.h>
#include <qtutils/disable_utils_warnings.h>
#include <QCoreApplication>


namespace qtutils { namespace ui{


#define QTUTILS_UI_WIDGETS_CONTAINER_IS_VIS_KEY_BEG  "qtutils/ui/widgets_container/"
#define QTUTILS_UI_WIDGETS_CONTAINER_IS_VIS_KEY_END  "/isVisible"


static void DefaultAppExiter(void){
    QMetaObject::invokeMethod(qApp,[](){
        QCoreApplication::quit();
    });
}


static void DefaultWidgetShow(QObject* a_pWidget){
    QWidget* const pWidget = dynamic_cast<QWidget*>(a_pWidget);
    if(pWidget){
        pWidget->show();
    }
}


static void DefaultWidgetHide(QObject* a_pWidget){
    QWidget* const pWidget = dynamic_cast<QWidget*>(a_pWidget);
    if(pWidget){
        pWidget->hide();
    }
}



WidgetsContainerBase::~WidgetsContainerBase()
{
    const size_t cunCount(m_vWidgets.size());
    for(size_t i(0); i<cunCount;++i){
        delete m_vWidgets[i];
    }
}


WidgetsContainerBase::WidgetsContainerBase(const AppExiter& a_appExiter)
    :
      m_appExiter(a_appExiter?a_appExiter:(&DefaultAppExiter)),
      m_index(-1),
      m_actionExit(QIcon(":/qtutils/exit_app.png"),"&Exit"),
      m_actionShowOrHide(QIcon(":/qtutils/hide_password.png"),"&Hide")
{
    m_flags.wr_all = CPPUTILS_BISTATE_MAKE_ALL_BITS_FALSE;
    ConnectSignalsQuBase();
    m_contextMenu.addAction(&m_actionExit);
#ifdef QTUTILS_UI_WC_SYSTRY_NEEDED
    m_tryIcon.setContextMenu(&m_contextMenu);
#endif
}


void WidgetsContainerBase::AddNewWidget(QObject* a_pWidget,
                                        const TypeWCShowHide& a_show, const TypeWCShowHide& a_hide,
                                        bool a_hasHideFromSysMenu)
{
    const TypeWCShowHide show = a_show?a_show:(&DefaultWidgetShow);
    const TypeWCShowHide hide = a_hide?a_hide:(&DefaultWidgetHide);
    SWidget* const pNextWidget = new SWidget(a_pWidget,show,hide,a_hasHideFromSysMenu);
    m_vWidgets.push_back(pNextWidget);
    const size_t cunIndex = m_vWidgets.size();
    const QString visKey = QTUTILS_UI_WIDGETS_CONTAINER_IS_VIS_KEY_BEG + QString::number(cunIndex) + QTUTILS_UI_WIDGETS_CONTAINER_IS_VIS_KEY_END;
    ::qtutils::Settings aSettings;
    const bool isVisible = aSettings.value(visKey,true).toBool();
    pNextWidget->flags.wr.isVisible = isVisible?CPPUTILS_BISTATE_MAKE_BITS_TRUE:CPPUTILS_BISTATE_MAKE_BITS_FALSE;
    if(m_index<0){
        SwitchToWidgetPrivate(0,false);
    }
}


void WidgetsContainerBase::SetWidgetVisible(size_t a_index, bool a_isVisible)
{
    SWidget* const pWidget = m_vWidgets[a_index];
    pWidget->flags.wr.isVisible = a_isVisible?CPPUTILS_BISTATE_MAKE_BITS_TRUE:CPPUTILS_BISTATE_MAKE_BITS_FALSE;
    const QString visKey = QTUTILS_UI_WIDGETS_CONTAINER_IS_VIS_KEY_BEG + QString::number(a_index) + QTUTILS_UI_WIDGETS_CONTAINER_IS_VIS_KEY_END;
    ::qtutils::Settings aSettings;
    aSettings.setValue(visKey,a_isVisible);
    if(a_index==static_cast<size_t>(m_index)){
        if(a_isVisible){
            if(m_flags.rd.isVisible_true){
                (pWidget->m_show)(pWidget->pWidget);
            }

            m_actionShowOrHide.setText("&Hide");
            m_actionShowOrHide.setIcon(QIcon(":/qtutils/hide_password.png"));

        }
        else{
            (pWidget->m_hide)(pWidget->pWidget);
            m_actionShowOrHide.setText("&Show");
            m_actionShowOrHide.setIcon(QIcon(":/qtutils/show_password.png"));
        }
    }  //  if(a_index==static_cast<size_t>(m_index)){
}


void WidgetsContainerBase::SetWidgetHasHideFromSysMenu(size_t a_index,bool a_hasHideFromSysMenu)
{
    SWidget* const pWidget = m_vWidgets[a_index];
    pWidget->flags.wr.hasHideFromSysMenu = a_hasHideFromSysMenu?CPPUTILS_BISTATE_MAKE_BITS_TRUE:CPPUTILS_BISTATE_MAKE_BITS_FALSE;

    if(a_index==static_cast<size_t>(m_index)){
        if(a_hasHideFromSysMenu){
            //m_contextMenu.removeAction(&m_actionShowOrHide);
            m_contextMenu.insertAction(&m_actionExit,&m_actionShowOrHide);
        }
        else{
            m_contextMenu.removeAction(&m_actionShowOrHide);
        }
    }  //  if(a_index==static_cast<size_t>(m_index)){
}


bool WidgetsContainerBase::SwitchToWidget(size_t a_index)
{
    if(a_index>=m_vWidgets.size()){
        return false;
    }

    if(a_index==static_cast<size_t>(m_index)){
        return true;
    }

    bool bOldHasHideFromSysMenu = false;

    if(m_index>=0){
        SWidget* const pCurWidget = m_vWidgets[m_index];
        (pCurWidget->m_hide)(pCurWidget->pWidget);
        bOldHasHideFromSysMenu = pCurWidget->flags.rd.hasHideFromSysMenu_true?true:false;
    }

    SwitchToWidgetPrivate(a_index,bOldHasHideFromSysMenu);
    return true;
}


void WidgetsContainerBase::SwitchToWidgetPrivate(size_t a_index, bool a_hasHideFromSysMenu)
{
    SWidget* const pNewWidget = m_vWidgets[a_index];

    if(a_hasHideFromSysMenu){
        if(pNewWidget->flags.rd.hasHideFromSysMenu_false){
            m_contextMenu.removeAction(&m_actionShowOrHide);
        }
    }
    else{
        if(pNewWidget->flags.rd.hasHideFromSysMenu_true){
            m_contextMenu.insertAction(&m_actionExit,&m_actionShowOrHide);
        }
    }

    if(pNewWidget->flags.rd.isVisible_false){
        m_actionShowOrHide.setText("&Show");
        m_actionShowOrHide.setIcon(QIcon(":/qtutils/show_password.png"));
    }
    else{
        m_actionShowOrHide.setText("&Hide");
        m_actionShowOrHide.setIcon(QIcon(":/qtutils/hide_password.png"));
        if(m_flags.rd.isVisible_true){
            (pNewWidget->m_show)(pNewWidget->pWidget);
        }
    }

    m_index = static_cast<ptrdiff_t>(a_index);
}


const WidgetsContainerBase::SWidget& WidgetsContainerBase::getWidgetNoCheck(size_t a_index)const
{
    return *(m_vWidgets[a_index]);
}


WidgetsContainerBase::SWidget& WidgetsContainerBase::getWidgetNoCheck(size_t a_index)
{
    return *(m_vWidgets[a_index]);
}


const WidgetsContainerBase::SWidget* WidgetsContainerBase::getWidgetCheck(size_t a_index)const
{
    const ptrdiff_t cunWidgsCount = static_cast<ptrdiff_t>(m_vWidgets.size());
    if((m_index<0) || (m_index>=cunWidgsCount)){
        return nullptr;
    }
    return m_vWidgets[a_index];
}


WidgetsContainerBase::SWidget* WidgetsContainerBase::getWidgetCheck(size_t a_index)
{
    const ptrdiff_t cunWidgsCount = static_cast<ptrdiff_t>(m_vWidgets.size());
    if((m_index<0) || (m_index>=cunWidgsCount)){
        return nullptr;
    }
    return m_vWidgets[a_index];
}


bool WidgetsContainerBase::isVisible()const
{
    return m_flags.rd.isVisible_true?true:false;
}


void WidgetsContainerBase::SetIconstForContextMenu(const QString& a_logo, const QString& a_tooltip,const QString& a_exitLogo)
{
    m_tryIcon.setIcon(QIcon(a_logo));
    m_tryIcon.setToolTip(a_tooltip);
    m_actionExit.setIcon(QIcon(a_exitLogo));
}


QMenu* WidgetsContainerBase::contextMenu()
{
    return &m_contextMenu;
}


#ifdef QTUTILS_UI_WC_SYSTRY_NEEDED


QSystemTrayIcon* WidgetsContainerBase::tryIcon()
{
    return &m_tryIcon;
}

#endif  //  #ifdef QTUTILS_UI_WC_SYSTRY_NEEDED


void WidgetsContainerBase::ConnectSignalsQuBase()
{
    connect(&m_actionExit,&QAction::triggered,this,[this](){
        m_appExiter();
    });

    connect(&m_actionShowOrHide,&QAction::triggered,this,[this](){
        const ptrdiff_t cunWidgsCount = static_cast<ptrdiff_t>(m_vWidgets.size());
        if((m_index<0) || (m_index>=cunWidgsCount)){
            return;
        }

        SWidget* const pCurWidget = m_vWidgets[m_index];
        if(pCurWidget->flags.rd.isVisible_false){
            pCurWidget->flags.wr.isVisible = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
            m_actionShowOrHide.setText("&Hide");
            m_actionShowOrHide.setIcon(QIcon(":/qtutils/hide_password.png"));
            if(m_flags.rd.isVisible_true){
                (pCurWidget->m_show)(pCurWidget->pWidget);
            }

            const QString visKey = QTUTILS_UI_WIDGETS_CONTAINER_IS_VIS_KEY_BEG + QString::number(m_index) + QTUTILS_UI_WIDGETS_CONTAINER_IS_VIS_KEY_END;
            ::qtutils::Settings aSettings;
            aSettings.setValue(visKey,true);
        }
        else{
            pCurWidget->flags.wr.isVisible = CPPUTILS_BISTATE_MAKE_BITS_FALSE;
            m_actionShowOrHide.setText("&Show");
            m_actionShowOrHide.setIcon(QIcon(":/qtutils/show_password.png"));
            (pCurWidget->m_hide)(pCurWidget->pWidget);

            const QString visKey = QTUTILS_UI_WIDGETS_CONTAINER_IS_VIS_KEY_BEG + QString::number(m_index) + QTUTILS_UI_WIDGETS_CONTAINER_IS_VIS_KEY_END;
            ::qtutils::Settings aSettings;
            aSettings.setValue(visKey,true);
        }
    });


#ifdef QTUTILS_UI_WC_SYSTRY_NEEDED
    connect(&m_tryIcon,&QSystemTrayIcon::activated,qApp,[this](QSystemTrayIcon::ActivationReason a_reason){
        switch(a_reason){
        case QSystemTrayIcon::Context:{
            // todo: implement this
        }break;
        case QSystemTrayIcon::DoubleClick:{
            const ptrdiff_t cunWidgsCount = static_cast<ptrdiff_t>(m_vWidgets.size());
            if((m_index<0) || (m_index>=cunWidgsCount)){
                return;
            }

            SWidget* const pCurWidget = m_vWidgets[m_index];
            pCurWidget->flags.wr.isVisible = CPPUTILS_BISTATE_MAKE_BITS_TRUE;
            m_actionShowOrHide.setText("&Hide");
            m_actionShowOrHide.setIcon(QIcon(":/qtutils/hide_password.png"));
            if(m_flags.rd.isVisible_true){
                (pCurWidget->m_show)(pCurWidget->pWidget);
            }
        }break;
        default:
            break;
        }
    });
#endif  //  #ifdef QTUTILS_UI_WC_SYSTRY_NEEDED

}


void WidgetsContainerBase::show()
{
    m_flags.wr.isVisible = CPPUTILS_BISTATE_MAKE_BITS_TRUE;


#ifdef QTUTILS_UI_WC_SYSTRY_NEEDED
    m_tryIcon.show();
#endif

    const ptrdiff_t cunWidgsCount = static_cast<ptrdiff_t>(m_vWidgets.size());
    if((m_index<0) || (m_index>=cunWidgsCount)){
        return;
    }

    bool isVisible;
    ::qtutils::Settings aSettings;
    QString visKey;
    const size_t cunCount = m_vWidgets.size();
    for(size_t unIndex(0); unIndex<cunCount;++unIndex){
        visKey = QTUTILS_UI_WIDGETS_CONTAINER_IS_VIS_KEY_BEG + QString::number(unIndex) + QTUTILS_UI_WIDGETS_CONTAINER_IS_VIS_KEY_END;
        isVisible = aSettings.value(visKey,true).toBool();
        m_vWidgets[unIndex]->flags.wr.isVisible = isVisible?CPPUTILS_BISTATE_MAKE_BITS_TRUE:CPPUTILS_BISTATE_MAKE_BITS_FALSE;
    }

    SwitchToWidgetPrivate(m_index,false);

    m_flags.wr.showCalled = CPPUTILS_BISTATE_MAKE_BITS_TRUE;

}


void WidgetsContainerBase::hide()
{

#ifdef QTUTILS_UI_WC_SYSTRY_NEEDED
    m_tryIcon.hide();
#endif

    const ptrdiff_t cunWidgsCount = static_cast<ptrdiff_t>(m_vWidgets.size());
    if((m_index<0) || (m_index>=cunWidgsCount)){
        return;
    }

    SWidget* const pCurWidget = m_vWidgets[m_index];

    if(pCurWidget->flags.rd.isVisible_true){
        (pCurWidget->m_hide)(pCurWidget->pWidget);
    }

    m_flags.wr.isVisible = CPPUTILS_BISTATE_MAKE_BITS_FALSE;
    m_flags.wr.showCalled = CPPUTILS_BISTATE_MAKE_BITS_FALSE;

}


WidgetsContainerBase::SWidget::SWidget(QObject* a_pWidget, const TypeWCShowHide& a_show, const TypeWCShowHide& a_hide, bool a_hasHideFromSysMenu)
    : pWidget(a_pWidget), m_show(a_show), m_hide(a_hide)
{
    this->flags.wr_all = CPPUTILS_BISTATE_MAKE_ALL_BITS_FALSE;
    this->flags.wr.hasHideFromSysMenu = a_hasHideFromSysMenu?CPPUTILS_BISTATE_MAKE_BITS_TRUE:CPPUTILS_BISTATE_MAKE_BITS_FALSE;
}


bool WidgetsContainerBase::SWidget::isVisible()const
{
    return (this->flags.rd.isVisible_true) ? true : false;
}


bool WidgetsContainerBase::SWidget::hasHideFromSysMenu()const
{
    return (this->flags.rd.hasHideFromSysMenu_true) ? true : false;
}


}}  //  namespace qtutils { namespace ui{
