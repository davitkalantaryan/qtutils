//
// file:            tabwidget.hpp
// path:			include/qtutils/ui/tabwidget.hpp
// created on:		2022 Aug 16
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_TABWIDGET_HPP
#define QTUTILS_INCLUDE_QTUTILS_UI_TABWIDGET_HPP

#include <qtutils/export_symbols.h>

#ifdef CPPUTILS_HASH_VHASH_DEFINED

#include <cpputils/hash/vhash.hpp>
#include <qtutils/disable_utils_warnings.h>
#include <QWidget>
#include <QLabel>


namespace qtutils { namespace ui{


class QTUTILS_EXPORT Tab;
class QTUTILS_EXPORT TabBar;
struct STabData{
    Tab*     pTab;
    QWidget* pWidget;
    // maybe QIcon* , etc.
};
struct STabHash{
    size_t operator()(const STabData& a_dt) const{
        return reinterpret_cast<size_t>(a_dt.pTab);
    }
};
struct STabEq{
    bool operator()(const STabData& a_lhs, const STabData& a_rhs) const{
        return a_lhs.pTab == a_rhs.pTab;
    }
};
typedef ::cpputils::hash::VSet<STabData,STabHash,STabEq> TypeHash;


class QTUTILS_EXPORT TabScene : public QWidget
{
public:
    template<typename... Targs>
    TabScene(Targs... a_args);
    virtual ~TabScene() override;

protected:
    virtual void    resizeEvent(QResizeEvent* a_event) override;

protected:
    QWidget*    m_pTabActiveWidget;

    friend class TabBar;
};


class QTUTILS_EXPORT TabBar : public QWidget
{
public:
    TabBar(TabScene* a_sceneWidget, QWidget* a_parent=nullptr);
    virtual ~TabBar() override;

    template<typename... Targs>
    Tab* addTab(QWidget *page, Targs... a_args);
    Tab* addTabRaw(QWidget *page, Tab* a_pTab);
    void setCurrentIndex(int index);
    int  currentIndex()const;
    const QString& styleSheetForSelected()const;
    const QString& styleSheetNonSelected()const;
    void SetStyleSheetForSelected(const QString&);
    void SetStyleSheetNonSelected(const QString&);
    int  tabsWidth()const;
    void SetTabsWidth(int a_tabsWidth);

private:
    void setCurrentIndexRaw(int index);
    void OrderAllTabs();

protected:
    TabScene*const      m_sceneWidget;
    QString             m_styleSheetForSelected;
    QString             m_styleSheetNonSelected;
    TypeHash            m_tabs;
    int                 m_currentTab;
    int                 m_tabsWidth;

    friend class Tab;
};


class QTUTILS_EXPORT Tab : public QLabel
{
public:
    template<typename... Targs>
    Tab(Targs... a_args);
    virtual ~Tab() override;

    int index()const;
    int isSelected()const;

    const QString& styleSheetForSelected()const;
    const QString& styleSheetNonSelected()const;
    void SetStyleSheetForSelected(const QString&);
    void SetStyleSheetNonSelected(const QString&);

protected:
    virtual void	mousePressEvent(QMouseEvent* event) override;

private:
    QString         m_styleSheetForSelected;
    QString         m_styleSheetNonSelected;
    int             m_index;
    int             m_isSelected : 2;
    int             m_reserved01 : 30;

    friend class TabBar;
};


}}  //  namespace qtutils { namespace ui{


#ifndef QTUTILS_INCLUDE_QTUTILS_UI_TABWIDGET_IMPL_HPP
#include "tabwidget.impl.hpp"
#endif

#endif  //  #ifdef CPPUTILS_HASH_VHASH_DEFINED


#endif  //  #ifndef QTUTILS_INCLUDE_QTUTILS_UI_TABWIDGET_HPP
