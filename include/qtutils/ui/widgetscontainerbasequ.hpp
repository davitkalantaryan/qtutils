//
// repo:            qtutils
// file:            widgetscontainer.hpp
// path:			include/qtutils/ui/widgetscontainer.hpp
// created on:		2023 Jul 02
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#pragma once

#include <qtutils/export_symbols.h>
#include <cinternal/bistateflags.h>
#include <functional>
#include <vector>
#include <qtutils/disable_utils_warnings.h>
#include <QObject>
#include <QString>
#include <QAction>
#include <QMenu>
#ifdef QTUTILS_UI_WC_SYSTRY_NEEDED
#include <QSystemTrayIcon>
#endif

namespace qtutils { namespace ui{

#define QTUTILS_UI_WIDGETSCONTAINER_BISTATE_FLAGS_CNT           3
#define QTUTILS_UI_WIDGETSCONTAINER_BISTATE_RESERVED_BITS_CNT   (64-2*QTUTILS_UI_WIDGETSCONTAINER_BISTATE_FLAGS_CNT)

class QTUTILS_UI_EXPORT WidgetsContainerBase : public QObject
{
public:
    union BistateFlags;
    struct SWidget;
    typedef ::std::function<void(QObject*)> TypeWCShowHide;
    typedef ::std::function<void(void)> AppExiter;

public:
    virtual ~WidgetsContainerBase() override;
    WidgetsContainerBase(const AppExiter& a_appExiter);
    
public slots:
    virtual void show();
    void hide();

public:
    void AddNewWidget(QObject* a_pWidget,
                      const TypeWCShowHide& a_show=nullptr, const TypeWCShowHide& a_hide=nullptr,
                      bool a_hasHideFromSysMenu=true);
    bool SwitchToWidget(size_t a_index); // checks index
    const SWidget& getWidgetNoCheck(size_t a_index)const;
    SWidget& getWidgetNoCheck(size_t a_index);
    const SWidget* getWidgetCheck(size_t a_index)const; // null means failure
    SWidget* getWidgetCheck(size_t a_index); // null means failure
    bool isVisible()const;
    void SetIconstForContextMenu(const QString& a_logo, const QString& a_tooltip,const QString& a_exitLogo);
    QMenu*  contextMenu();
    void SetWidgetVisible(size_t a_index,bool a_isVisible);
    void SetWidgetHasHideFromSysMenu(size_t a_index,bool a_hasHideFromSysMenu);

#ifdef QTUTILS_UI_WC_SYSTRY_NEEDED
    QSystemTrayIcon*  tryIcon();
#endif

protected:
    void SwitchToWidgetPrivate(size_t a_index, bool a_hasHideFromSysMenu);

private:
    void ConnectSignalsQuBase();

protected:
    const AppExiter         m_appExiter;
    ptrdiff_t               m_index;
    ::std::vector<SWidget*> m_vWidgets;
    QMenu                   m_contextMenu;
    QAction                 m_actionExit;
    QAction                 m_actionShowOrHide;

#ifdef QTUTILS_UI_WC_SYSTRY_NEEDED
    QSystemTrayIcon         m_tryIcon;
#endif
    
public:
    union BistateFlags {
        uint64_t  wr_all;
        struct {
            CPPUTILS_BISTATE1(isVisible);
            CPPUTILS_BISTATE1(hasHideFromSysMenu);
            CPPUTILS_BISTATE1(showCalled);
            uint64_t    reserved01 : QTUTILS_UI_WIDGETSCONTAINER_BISTATE_RESERVED_BITS_CNT;
        }rd;
        struct {
            CPPUTILS_BISTATE2(isVisible);
            CPPUTILS_BISTATE2(hasHideFromSysMenu);
            CPPUTILS_BISTATE2(showCalled);
            uint64_t    reserved01 : QTUTILS_UI_WIDGETSCONTAINER_BISTATE_RESERVED_BITS_CNT;
        }wr;
    };

    struct SWidget{
        QObject*const           pWidget;
        const TypeWCShowHide    m_show;
        const TypeWCShowHide    m_hide;
    private:
        BistateFlags            flags;

        SWidget(QObject* a_pWidget, const TypeWCShowHide& a_show, const TypeWCShowHide& a_hide, bool a_hasHideFromSysMenu);
        SWidget(const SWidget&)=delete;
        SWidget& operator=(const SWidget&)=delete;
    public:
        bool isVisible()const;
        bool hasHideFromSysMenu()const;
        void SetHasHideFromSysMenu(bool a_hasHideFromSysMenu);

        friend class WidgetsContainerBase;
    };

protected:
    BistateFlags    m_flags;

};


}}  //  namespace qtutils { namespace ui{
