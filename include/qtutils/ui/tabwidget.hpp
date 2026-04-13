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

#include <cpputils/hash/templ/vecthash.hpp>
#include <cinternal/disable_compiler_warnings.h>
#include <qtutils/disable_utils_warnings.h>
#include <QWidget>
#include <QLabel>
#include <cinternal/undisable_compiler_warnings.h>


namespace qtutils { namespace ui{


class QTUTILS_EXPORT Tab;
class QTUTILS_EXPORT TabBar;
struct STabData{
    Tab*     pTab;
    QWidget* pWidget;
    // maybe QIcon* , etc.
};
struct SKeyTab : public ::cpputils::hash::bh::SKeyExtBase<SKeyTab, STabData>
{
public:
    using ::cpputils::hash::bh::SKeyExtBase<SKeyTab, STabData>::SKeyExtBase;
    uint64_t hash()const override;
    bool areTheKeysSame(const CKeyBase& a_key2) const override;
};
typedef ::cpputils::hash::templ::MtVectHash<STabData,STabData,SKeyTab>  TypeHash;
typedef TypeHash::TypeRawHash                                           TypeRawHash;


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
    TabBar(TypeRawHash* CPPUTILS_ARG_NN a_rawHash_p, TabScene* a_sceneWidget, QWidget* a_parent=nullptr);
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

private:
    TabBar(const TabBar&)=delete;
    TabBar(TabBar&&)=delete;
    TabBar& operator=(const TabBar&)=delete;
    TabBar& operator=(TabBar&&)=delete;

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
