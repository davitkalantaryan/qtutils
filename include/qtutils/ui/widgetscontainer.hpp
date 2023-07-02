//
// repo:            qtutils
// file:            widgetscontainer.hpp
// path:			include/qtutils/ui/widgetscontainer.hpp
// created on:		2023 Jul 02
// created by:		Davit Kalantaryan (davit@focust.ai)
//

#pragma once

#include <qtutils/export_symbols.h>
#include <qtutils/disable_utils_warnings.h>
#include <QAction>
#ifdef FOCUST_P01_SYSTRY_NEEDED
#include <QSystemTrayIcon>
#include <QMenu>
#endif

namespace qtutils { namespace ui{


class QTUTILS_EXPORT WidgetsContainer;
typedef QWidget WidgType;

//template <typename WidgType>
class WCMainWindow : public WidgType
{
public:
    virtual ~WCMainWindow();
    WCMainWindow(WidgetsContainer* a_widgsContainer);

    virtual void InitAndShow();

protected:
    WidgetsContainer*const m_pWgtsContainer;
};


class QTUTILS_EXPORT WidgetsContainer : public QObject
{
public:
    virtual ~WidgetsContainer() override;
    template <typename WidgType>
    WidgetsContainer(QWidget* a_pLoginWnd, WCMainWindow<WidgType>* a_pMainWindow);
    
    void show();
    void MainWindowCloseButtonPressed();
	void SwitchMainWindowToHidden();

private:
	void SwitchMainWindowToShow();
    void ConnectSignals();

protected:
    QWidget*const           m_pLoginWnd;
    QWidget*const           m_pMainWnd;
#ifdef FOCUST_P01_SYSTRY_NEEDED
    QSystemTrayIcon         m_tryIcon;
    QMenu                   m_contextMenu;
#endif
    QAction                 m_actionShow;
    QAction                 m_actionHide;
    QAction*                m_pActionShowOrHide;
    
    union{
		uint64_t wr_all;
		struct{
			uint64_t	displayOrNotMainWindows_true : 1;
			uint64_t	displayOrNotMainWindows_false : 1;
			uint64_t	reserved64bit : 62;
		}rd;
		struct{
			uint64_t	displayOrNotMainWindows : 2;
			uint64_t	reserved64bit : 62;
		}wr;
	}m_flags;
};


}}  //  namespace qtutils { namespace ui{
