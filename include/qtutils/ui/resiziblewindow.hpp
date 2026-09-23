//
// file:            resizible_window.hpp
// path:			include/focust/ui/resizible_window.hpp
// created on:		2021 Dec 21
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_RESIZIBLEWINDOW_HPP
#define QTUTILS_INCLUDE_RESIZIBLEWINDOW_HPP


#include <qtutils/export_symbols.h>
#include <cinternal/bistateflags.h>
#include <cinternal/disable_compiler_warnings.h>
#include <stdint.h>
#include <qtutils/disable_utils_warnings.h>
#include <QCloseEvent>
#include <QHideEvent>
#include <QMoveEvent>
#include <QResizeEvent>
#include <QShowEvent>
#include <cinternal/undisable_compiler_warnings.h>


namespace qtutils { namespace ui{

#ifndef QTUTILS_RSZ_WND_INIT_AND_SHOW_OVERRIDE
#define QTUTILS_RSZ_WND_INIT_AND_SHOW_OVERRIDE
#endif


template <typename WidgetType>
class ResizibleWindow : public WidgetType
{    
public:
	template<typename... Targs>
    ResizibleWindow(Targs... a_args);
    virtual ~ResizibleWindow() override = default;
    
    const QString& settingsKey()const noexcept;
    void show();
	    
protected:
    virtual void showEvent(QShowEvent *event) override;
	virtual void closeEvent(QCloseEvent *event) override;
	virtual void hideEvent(QHideEvent *event) override;
	
private:
    inline void saveSizesInline() const;
    inline bool LoadSizesBecauseOfFirstShowCallInline(bool a_bFromShow);
	
protected:
    static uint64_t	sm_numberOfInstances;
public:
    const uint64_t  m_instanceNumber;
private:
	QString		m_settingsKey;
    CPPUTILS_BISTATE_FLAGS_UN(
        loadSizesCalled,
        hasCloseAfterShow
    )m_flags;
};


}}  // namespace qtutils { namespace ui{


#ifndef QTUTILS_INCLUDE_RESIZIBLEWINDOW_IMPL_HPP
#include <qtutils/ui/impl/qtutils_ui_resiziblewindow.impl.hpp>
#endif


#endif  // #ifndef QTUTILS_INCLUDE_RESIZIBLEWINDOW_HPP
